/* ================================================
 *  Projeto Final Integrado
 *  Teoria de Controle 2 e Microcontroladores (2023.2)
 *  Controle Ventoinha
 *  Alunos:
 *  Jonas e Vinícius Bauer
 * ================================================ */

#include <xc.h>
#include "lib_pic.h"       // define chip, kit, configbits, millis, delay
#include "lib_interruptVectors.h" // Define os vetores das interrupções
#include "lib_timer0.h"    // Timer 0 (usado para medir tempo, com interrupção)
#include "lib_timer1.h"    // Timer 1 (usado no tacômetro)
#include "lib_timer2.h"    // Timer 2 (usado para PWM)
#include "lib_lcd.h"       // funções do display LCD
#include "lib_keyboard.h"  // botões e teclado matricial
#include "lib_pwm.h"       // Funções PWM
#include "lib_convAD.h"    // Funções do conversor AD

//---------------------------------------------------------

#define defaultTsValue 100 // default value for Ts in milliseconds
uint16_t contTs=defaultTsValue; // counter for sample time Ts
uint16_t tk; // timer position: t[k]
uint16_t vk; // timer velocity: v[k]
uint16_t sp=25; // setpoint
float ukx, uk_pi, uk_pid;
float uk=0.0, uk_1 = 0.0, ek = 0.0, ek_1 = 0.0, ek_2 = 0.0;
uint16_t contP1=0, contP2=0, contP3=0;
int ctrl = 0;

// ---------------------------------------------------------------------------------------------------------------
// Ações do Gerais

void setInicial(){
    // Atualização dos valores desejados de setpoint, duty e velocidade
    lcd_string("\fSP=000 *=config\nVEL=000 duty=000");
    keyboardScan();
    lcd_command(0x83); lcd_display3(sp);
    lcd_command(0xCD); lcd_display3(ukx);
    lcd_command(0xC4); lcd_display3(vk);
}

//----------------------------------------------------------------------------------------------------------------

void decodifSP(){
    // Função para separar o setpoint em centena, dezena e unidade para facilitar a visualização e futura edição 
    lcd_conv3(sp);
    contP1 = lcd.cent-'0';
    contP2 = lcd.dez-'0';
    contP3 = lcd.un-'0';
}

// ---------------------------------------------------------------------------------------------------------------

void setPoints(){
    // Função que mostra os valores de centena, dezena e unidade que vão ser atualizados
    lcd_string("\fConfig setpoint\nP1=0  P2=0  P3=0");
    // keyboardScan();
    decodifSP();
    lcd_command(0xC3); lcd_data(contP1+'0');
    lcd_command(0xC9); lcd_data(contP2+'0');
    lcd_command(0xCF); lcd_data(contP3+'0');
}

// ----------------------------------------------------------------------------------------------------------------
// Ações do Cursor

void acaoA1(){
    // Função para incrementar na centena e atualizar o valor
    // keyboardScan();
    lcd_string("\f1=[+]  2=[-]\nP1=0  *=ok #=ret");
    contP1++;
    lcd_command(0xC3); lcd_data(contP1+'0');
}

void acaoA2(){
    // Função para incrementar na dezena e atualizar o valor
    // keyboardScan();
    lcd_string("\f1=[+]  2=[-]\nP2=0  *=ok #=ret");
    contP2++;
    lcd_command(0xC3); lcd_data(contP2+'0');
}

void acaoA3(){
    // Função para incrementar na unidade e atualizar o valor
    // keyboardScan();
    lcd_string("\f1=[+]  2=[-]\nP3=0  *=ok #=ret");
    contP3++;
    lcd_command(0xC3); lcd_data(contP3+'0');
}

void acaoB1(){
    // Função para decrementar na centena e atualizar o valor
    // keyboardScan();
    lcd_string("\f1=[+]  2=[-]\nP1=0  *=ok #=ret");
    if(contP1>0) contP1--;
    lcd_command(0xC3); lcd_data(contP1+'0');
}

void acaoB2(){
    // Função para decrementar na dezena e atualizar o valor
    // keyboardScan();
    lcd_string("\f1=[+]  2=[-]\nP2=0  *=ok #=ret");
    if(contP2>0) contP2--;
    lcd_command(0xC3); lcd_data(contP2+'0');
}

void acaoB3(){
    // Função para decrementar na unidade e atualizar o valor
    // keyboardScan();
    lcd_string("\f1=[+]  2=[-]\nP3=0  *=ok #=ret");
    if(contP3>0) contP3--;
    lcd_command(0xC3); lcd_data(contP3+'0');
}

// ----------------------------------------------------------------------------------------------------------------

void spResult(){
    // Função que junta as variáveis de centena, dezena e unidade para formar o setpoint desejado
    // keyboardScan();
    sp = (contP1*100) + (contP2*10) + (contP3);
    lcd_string("\fSetpoint=   \n*=conf  #=cancel");
    lcd_command(0x89); lcd_display3(sp);
}

// ----------------------------------------------------------------------------------------------------------------
// Máquina de estados implementada para montar a IHM com suas interações

/*=================================================================================================================

Funcionamento da IHM:

Nossa interface homem-máquina (IHM) opera por meio de uma máquina de estados que foi implementada com 9 telas
personalizadas, proporcionando uma interação eficiente com o usuário. Inicialmente, oferecemos a opção de
controle discreto, aplicável tanto para controle de velocidade PI quanto PID.

Em seguida, disponibilizamos a capacidade de edição do setpoint desejado. Essa edição é realizada por meio de
funções de incremento e decremento nas centenas, dezenas e unidades, permitindo que o usuário ajuste precisamente o
valor desejado. Essa etapa é importante para a obtenção da velocidade desejada por meio do controle.

Dessa forma, a tela principal, correspondente ao estado 1 da nossa máquina, apresenta os parâmetros essenciais para
o usuário, incluindo setpoint, velocidade e duty. Esses dados fornecem as informações necessárias para o controle
preciso do sistema, garantindo uma interação eficaz e intuitiva com a IHM.

=================================================================================================================*/

void fsmIHM(){
    // Máquina de Estado do Sistema
    volatile static byte estado = {0};
    switch (estado){
        case 1: // estado 1
            // Tela que ostra os parâmetros de setpoint, velocidade e duty
            keyboardScan();
            lcd_command(0x83); lcd_display3(sp);
            lcd_command(0xCD); lcd_display3(ukx);
            lcd_command(0xC4); lcd_display3(vk);
            if(tecla == '*'){estado=2; lcd_string("\fMuda controle?\n*=s #=n 0=cancel");} // vai para o estado 2 e oferece a opção de mudar de controle
        case 2: // estado 2
            // Tela que dá a opção de mudar o controle
            keyboardScan();
            if(tecla == '*'){estado=3; lcd_string("\fSelect Control\n1=PI    2=PID");} // vai para o estado 3 e dá as opções de controle PI ou PID
            if(tecla == '#'){estado=4; ctrl = 1; lcd_string("\fMuda setpoint?\n*=s #=n 0=cancel");} // vai para o estado 4 e dá opção de mudar o setpoint
            if(tecla == '0'){estado=1; setInicial();} // retorna ao estado 1 e mostra o sp, vel e duty
            break;
        case 3: // estado 3
            // Tela que mostra as opções de controle PI ou PID
            keyboardScan();
            if(tecla == '1'){estado=4; ctrl=1; lcd_string("\fMuda setpoint?\n*=s #=n 0=cancel");}// vai para o estado 4, escolhe o PI e dá opção de mudar o setpoint
            if(tecla == '2'){estado=4; ctrl=2; lcd_string("\fMuda setpoint?\n*=s #=n 0=cancel");}// vai para o estado 4, escolhe o PID e dá opção de mudar o setpoint
            break;
        case 4: // estado 4
            // Tela que mostra dá a opção de mudar o setpoint
            keyboardScan();
            if(tecla == '*'){estado=5; setPoints();}// vai para o estado 5 e mostra P1, P2 e P3 do setpoint
            if(tecla == '#'){estado=9; spResult();}// vai para o estado 9 e mostra o setpoint pedindo para confirmar
            if(tecla == '0'){estado=1; setInicial();}// retorna ao estado 1 e mostra o sp, vel e duty
            break;
        case 5: // estado 5
            //  Tela que mostra centena, dezena e unidade para configuração do setpoint
            keyboardScan();
            if(tecla == '*'){estado=6; lcd_string("\f1=[+]  2=[-]\nP1=0  *=ok #=ret");}// vai para o estado 6, dá as opções de incremento e decremento
            break;
        case 6: // estado 6: posição 1 = configuração da centena
            keyboardScan();
            if(tecla == '1'){estado=6; acaoA1();} // retorna ao estado atual com seu valor incrementado
            if(tecla == '2'){estado=6; acaoB1();} // retorna ao estado atual com seu valor decrementado
            if(tecla == '*'){estado=7; lcd_string("\f1=[+]  2=[-]\nP2=0  *=ok #=ret");} // vai para o estado 7 com o p1 atualizado
            if(tecla == '#'){estado=5; lcd_string("\fConfig setpoint\nP1=0  P2=0  P3=0");} // retorna ao estado 5 com o p1 atualizado
            break;
        case 7: // estado 7: posição 2 = configuração da dezena
            keyboardScan();
            if(tecla == '1'){estado=7; acaoA2();} // retorna ao estado atual com seu valor incrementado
            if(tecla == '2'){estado=7; acaoB2();} // retorna ao estado atual com seu valor decrementado
            if(tecla == '*'){estado=8; lcd_string("\f1=[+]  2=[-]\nP3=0  *=ok #=ret");} // vai para o estado 8 com o p2 atualizado
            if(tecla == '#'){estado=6; lcd_string("\f1=[+]  2=[-]\nP1=0  *=ok #=ret");} // retorna ao estado 6 com o p2 atualizado
            break;
        case 8: // estado 8: posição 3 = configuração da unidade
            keyboardScan();
            if(tecla == '1'){estado=8; acaoA3();} // retorna ao estado atual com seu valor incrementado
            if(tecla == '2'){estado=8; acaoB3();} // retorna ao estado atual com seu valor decrementado
            if(tecla == '*'){estado=9; spResult();} // vai para o estado 9 com o p3 atualizado
            if(tecla == '#'){estado=7; lcd_string("\f1=[+]  2=[-]\nP2=0  *=ok #=ret");} // retorna ao estado 6 com o p3 atualizado
            break;
        case 9: // estado 9
            // Tela que mostra o novo setpoint personalizado
            keyboardScan();
            if(tecla == '*'){estado=1; setInicial();} // vai para o estado 1, confirma o sp configurado e mostra o sp, vel e duty
            if(tecla == '#'){estado=4; lcd_string("\fMuda setpoint?\n*=s #=n 0=cancel");} // vai para o estado 4 e pergunta se deseja alterar os setpoint
            break;
        default: // estado 0
            // Ações inciais do display
            keyboardScan();
            if(tecla == '*'){estado=1; setInicial();} // vai para o estado 1 e mostra o sp, vel e duty
    }
}

//---------------------------------------------------------

void isr_timer1speed() {
    static uint16_t tk_1; // previous Timer 1 position: t[k-1]
    tk = readTimer1();    // get timer 1: t[k]
    vk = tk - tk_1;       // calculates difference (speed)
    tk_1 = tk;            // updates t[k-1]
}

//---------------------------------------------------------
// Timer 0 Interrupt function

void isr_Timer0() {
    T0_overflowFlag = 0;
    // sp=25;
    millis++;     // contador universal millis
    if(contDeadTime>0) contDeadTime--;  // contador autoregressivo do tempo morto
    if(--contTs == 0) { // Sample time Ts
        contTs = defaultTsValue;
        isr_timer1speed(); // captures timer 1 and calculates fan speed
        ek = (float)(sp - vk);
        uk_pi = uk_1 + 0.056*(ek - 0.85*ek_1);
        // uk_pid = uk_1 + ?*(ek - ?*ek_1 + ?*ek_2);
        // if(ctrl == 1){
        //     uk_pi = uk_1 + 0.03*(ek - 0.79*ek_1);
        //     uk = uk_pi;
        // }
        // else{
        // uk_pid = uk_1 + 0.05*(ek - 1.6*ek_1 + 0.68*ek_2);
        uk_pid = uk_1 + 0.01*(ek - 1.6*ek_1 + 0.68*ek_2);
        //     uk = uk_pid;
        // }
        uk = uk_pid;
        // if(uk>5) uk=5;
        // if(uk<0) uk=0;
        // ukx=(799*uk/5);
        ukx=(199*uk/5);
        // setDutyPwm1(ukx);
        CCPR1L = (unsigned char)ukx;
        uk_1 = uk;
        ek_2 = ek_1;
        ek_1 = ek;
    }
}

//---------------------------------------------------------

void setup() {
    di();  // disable interrupts
    TRISD = 0x00;   // to use LEDs
    setupTimer0(); T0_enableInterrupt = 1; T0_overflowFlag = 0;
    setupTimer1();
    setupLCD(); lcd_init(); lcd_cursorOff();
    setupKeyboard();
    setupPWM();
    setupConvAD();
    lcd_string("\fProjeto Final\nBauer e Jonas"); 
    ei();  // enable interrupts
}

//---------------------------------------------------------

void loop() {
    // teste1();
    // teste2();
    // teste3();
    // teste4();
    // teste5();
    // telaInicial();
    fsmIHM();
}