/* 
 * File:   mainE3.c
 * Author: Luiz Felix
 *
 * Created on 17 de Dezembro de 2020, 22:36
 */

#define _XTAL_FREQ 4000000
#include <xc.h>
#include <string.h>

#pragma config FOSC = HS   // Oscilador externo
#pragma config WDT = OFF        // Watchdog Timer desligado
#pragma config MCLRE = OFF      // Master Clear desligado
#pragma config PBADEN = OFF     // AD da porta B desligado

void Mensagem(long *verifica);
void setupTmr2();
void setupInt(void); // interrup��es TIMER2 e ADC
void setupADC(void); 
void setupPWM(void);
void inicializa_RS232();
void imprime(const char frase[]);
void escreve(char valor);

long contagem = 0; // vari�vel auxiliar
char linha[22];

void main(void) {

    TRISC = 0b10000000; // Porta C: RXD,TXD,RC5-RC1
               // TXD, RC1=ENA, RC4=IN1, RC5=IN2: pinos de sa�da
    TRISBbits.TRISB2 = 1; // RB2 (CH2) entrada para o sentido de rota��o 
    PORTC = 0; // Limpa portas configuradas como sa�da
    
    inicializa_RS232();
    setupADC();
    setupInt();
    setupPWM();
    ADCON0bits.GO = 1; //Inicia a convers�o AD
    
    while(1) { 
    }
}

void escreve(char valor){
    TXIF = 0;        // limpa flag que sinaliza envio completo.
    TXREG = valor;   // Envia caractere desejado � porta serial
    while(TXIF ==0); // espera caractere ser enviado
}

void imprime(const char frase[]){
    char indice= 0;               // �ndice da cadeia de caracteres
    char tamanho = strlen(frase); // tamanho total da cadeia a ser impressa
    while(indice<tamanho) {       // verifica se todos foram impressos
        escreve(frase[indice]);   // Chama rotina que escreve o caractere
        indice++;                 // incrementa �ndice
    }
}

void __interrupt() interrupcao(void){
    if(ADIF) {
        contagem = (ADRESH * 0x100) + ADRESL; // Transfere a leitura do AD
        contagem = contagem >> 2;     // rotacional 2 posi��es � direita 
                                      // (divide por 4) p/ ajustar aos 8 bits
        Mensagem(&contagem);          // chama fun��o que trata a mensagem
        CCPR2L = contagem;      
               
        ADIF = 0;                     // Desmarca flag da interrup��o ADC 
        ADCON0bits.GO = 1;      
    }
    if(PORTBbits.RB2 == 1) LATC=0b00100010; //sentido anti-hor�rio      
        else if (PORTBbits.RB2 == 0) LATC=0b00010010; //sentido hor�rio  
    //INT2IF = 0; //Limpa a Flag que aciona a INT2, sen�o as INT ficam bloqueadas

}

void Mensagem(long *verifica){ // potenci�metro vai de 0 a 250
    if(*verifica <50){ //abaixo de 20%
        imprime("Velocidade proxima de zero\n\r");
    }
    else if(*verifica >204){ //acima de 80%
        imprime("Velocidade proxima da nominal\n\r");
        if(*verifica>249)*verifica=249; //se potenciometro=100% (250) d� erro
    }
    else{
        imprime("Velocidade adequada\n\r");
    }
}

void setupInt(void) {
    GIE = 1;        // Habilita interrup��o global
    PEIE = 1;       // Habilita interrup��o de perif�ricos
    ADIE = 1;       // Habilita interrup��o do ADC
}

void setupPWM(void) {    
    TRISCbits.RC1 = 1;           // "desliga" bit de sa�da
    setupTmr2();                 // Configura timer 2
    CCP2CONbits.CCP2M = 0b1100;  // Modo PWM ativo
    CCPR2L = 128;                // Dutycycle% do PWM(0-255),portanto 128 = 50%
    TMR2IF = 0;                  // Limpa flag do TMR2 
    TMR2ON = 1;                  // Dispara o timer
    TRISCbits.RC1 = 0;           // "liga" bit de sa�da
}
void setupTmr2() {
    TMR2 = 0x00;   // Come�a a contar de 0
    PR2 = 249;   // at� 250 (conta 250 vezes + recarga autom�tica)
}

void setupADC(void){    
    TRISA = 0b00000010;  // Habilita pino A1 como entrada 
    ADCON2bits.ADCS = 0b110; // clock = Fosc/64
    ADCON2bits.ACQT = 0b010; // tempo aquisi��o = 4 Tad
    ADCON2bits.ADFM = 0b1;   // justificado � direita    
    ADCON1bits.VCFG = 0b00;  // Tens�es de refer�ncia: Vss e Vdd
    ADCON0bits.CHS = 0b0001; // Seleciona canal AN1
    ADCON0bits.ADON = 1;     // Conversor AD ligado
}

void inicializa_RS232() {
    RCSTA = 0x90; // Habilita porta serial, recep��o de 8 bits assinc
    int valor;
    TXSTA = 0x24; // modo assinc, transm 8 bits
    valor = (int) (((_XTAL_FREQ / 9600) - 16) / 16); //valor p/ gerar baud rate
    SPBRG = valor; //esse reg,carregado c/valor calculado,define o baud rate    
    RCIE = 1; // habilita interrup��o de recep��o
    TXIE = 0; // desabilita interrup��o de transmiss�o
}
