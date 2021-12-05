# Microcontrolador_PIC_18F4555_Projeto3
Uso do potenciômetro + ADC + PWM + Motor CC + Serial no microcontrolador PIC 18F4555

Atividade desenvolvida na disciplina Microprocessadores e Microcontroladores da minha graduação em Engenharia de Computação.

Acionamento do motor CC com PWM usando potenciômetro e conversor AD p/
variar a velocidade. Quando a velocidade ultrapassar 80% do valor máx, enviar
mensagem “Velocidade Próxima da Nominal” via serial. Quando velocidade menor
que 20%, enviar “Velocidade próxima de zero” via serial. Velocidade entre 20%
e 80%, enviar “Velocidade Adequada”. Sentido de rotação do motor CC definido
através do botão CH2 (0=sentido horário, 1= sentido anti-horário)

Para compilar o código, recomenda-se o uso da IDE MPLab-X https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-x-ide
