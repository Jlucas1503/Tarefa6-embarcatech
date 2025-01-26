#include "pico/stdlib.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include "stdio.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "pico/binary_info.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

const uint I2C_SDA = 14; // GPIO 14 - conexão com a porta I2C do display
const uint I2C_SCL = 15; //  GPIO 15


#define BTN_A_PIN 5

int A_state = 0;    //Botao A está pressionado?


struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
};

void SinalAberto(){
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);

    char *text1[] = {
        "Sinal aberto!",
        "Atravessar com",
        "cuidado!"};

        int y = 0;
        for (uint i = 0; i < count_of(text1); i++){
        ssd1306_draw_string(ssd, 5, y, text1[i]);
        y += 8;
        }
        render_on_display(ssd, &frame_area);


}

void SinalAtencao(){
     
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
    char *text2[] = {
        "Sinal de",
        "atencao!",
        "Prepare-se!"};

        int y = 0;
        for (uint i = 0; i < count_of(text2); i++){
        ssd1306_draw_string(ssd, 5, y, text2[i]);
        y += 8;
        }
        render_on_display(ssd, &frame_area);
}

void SinalFechado(){
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);    

    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
    char *text3[] = {
        "Sinal fechado!",
        "Aguarde"};

        int y = 0;
        for (uint i = 0; i < count_of(text3); i++){
        ssd1306_draw_string(ssd, 5, y, text3[i]);
        y += 8;
        }
        render_on_display(ssd, &frame_area);
}

int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}
int main(){
    
    stdio_init_all(); // INICIANDO UART
    printf("Iniciando comunicaçao serial!\n");

/* OLED /////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    
   /* struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };*/

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
   /* uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);*/

  
/* OLED /////////////////////////////////////////////////////////////////////////////////////////////////////////////// */


    // INICIANDO LEDS
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // INICIANDO BOTÄO
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    

    while(true){

        SinalAberto();
        A_state = WaitWithRead(8000);   //espera com leitura do botäo
        //sleep_ms(8000);


        if(A_state){               //ALGUEM APERTOU O BOTAO - SAI DO SEMAFORO NORMAL
            //SINAL AMARELO PARA OS CARROS POR 5s
            SinalAtencao();
            sleep_ms(5000);

            //SINAL VERMELHO PARA OS CARROS POR 10s
            SinalFechado();
            sleep_ms(10000);

        }else{                          //NINGUEM APERTOU O BOTAO - CONTINUA NO SEMAFORO NORMAL
                                      
            SinalAtencao();
            sleep_ms(2000);

            //SINAL VERMELHO PARA OS CARROS POR 15s
            SinalFechado();
            sleep_ms(8000);
        }
                
    }

    return 0;

}
