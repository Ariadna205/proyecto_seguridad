#include <xc.h>
#include <pic16f887.h>
#include <stdio.h>
#include <string.h>
#include "CONFIG.h"
#include "LCD.h"
#include "KEYPAD.h"


const char password[6] = "12345";  // Clave correcta
char pass_user[8];          // Buffer para almacenar la clave ingresada
unsigned char ingresos = 0;


void main() {
    char key = '0';  
    int intentos = 0;
    
    OSCCON = 0x71;  // Oscilador interno 8MHz
    ANSELH = 0x00;

    LCD_Init();     // Inicializa LCD
    keypad_Init();  // Inicializa Keypad
    
    
    TRISDbits.TRISD0 = 0;  // LED Verde
    TRISDbits.TRISD1 = 0;  // LED Azul
    TRISDbits.TRISD2 = 0;  // LED Rojo

    while (1) {
        LCD_Clear();
        LCD_String_xy(0, 0, "Ingresar Clave:");
        LCD_Command(0xC0);

        ingresos = 0;  // Reiniciar índice al comenzar a ingresar la clave
        do {
            key = keypad_getkey();
            if (key != 0) {
                LCD_Char('*');  // Enmascarar con '*'
                pass_user[ingresos++] = key;
            }
            __delay_ms(100);
        } while (ingresos < 5 || ingresos > 8);  // Clave entre 5 y 8 caracteres

        pass_user[ingresos] = '\0';  // Termina la cadena de la clave (nulo)

        if (strncmp(pass_user, password, 5) == 0) {
            LCD_Clear();
            LCD_String_xy(0, 0, "Clave Correcta"); // LED verde parpadeando
            for (int i = 0; i < 6; i++) {  // 3 segundos (6 ciclos de 500 ms)
                PORTDbits.RD0 = 1;  // LED Verde ON
                __delay_ms(500);
                PORTDbits.RD0 = 0;  // LED Verde OFF
                __delay_ms(500);
            }
            intentos = 0;  // Reiniciar contador de intentos fallidos
        } else {
            intentos++;
            LCD_Clear();
            LCD_String_xy(0, 0, "Clave Incorrecta");
                for (int i = 0; i < 4; i++) {  // 2 segundos (4 ciclos de 1s)
                PORTDbits.RD1 = 1;  // LED Azul ON
                __delay_ms(300);
                PORTDbits.RD1 = 0;  // LED Azul OFF
                __delay_ms(700);
            }

            if (intentos >= 3) {
                LCD_Clear();
                LCD_String_xy(0, 0, "Sistema Bloqueado");
                for (int i = 0; i < 16; i++) {  // 8 segundos (16 ciclos de 500 ms ON y 300 ms OFF)
                    PORTDbits.RD2 = 1;  // LED Rojo ON
                    __delay_ms(300);
                    PORTDbits.RD2 = 0;  // LED Rojo OFF
                    __delay_ms(500);
                }
                intentos = 0;  // Reiniciar sistema después del bloqueo
            }
        }
        __delay_ms(2000);  // Pausa antes de reiniciar
    }
}
