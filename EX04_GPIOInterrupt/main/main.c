/*
	Autor: Prof. Vagner Rodrigues
	Objetivo: Manipulação das GPIOs utilizando SDK-IDF com RTOS (FreeRTOS)
			  Utilizando interrupção externa
	Disciplina: IoT II
	Curso: Engenharia da Computação
*/

/*	Relação entre pinos da WeMos D1 R2 e GPIOs do ESP8266
	Pinos-WeMos		Função			Pino-ESP-8266
		TX			TXD				TXD/GPIO1
		RX			RXD				RXD/GPIO3
		D0			IO				GPIO16	
		D1			IO, SCL			GPIO5
		D2			IO, SDA			GPIO4
		D3			IO, 10k PU		GPIO0
		D4			IO, 10k PU, LED GPIO2
		D5			IO, SCK			GPIO14
		D6			IO, MISO		GPIO12
		D7			IO, MOSI		GPIO13
		D0			IO, 10k PD, SS	GPIO15
		A0			Inp. AN 3,3Vmax	A0
*/

/* Inclusão das Bibliotecas */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"

/* Definições e Constantes */
#define TRUE  1
#define FALSE 0
#define DEBUG TRUE 
#define LED_BUILDING	GPIO_NUM_2 
#define LED_1			GPIO_NUM_16 
#define LED_2			GPIO_NUM_5 
#define BUTTON			GPIO_NUM_4 
#define GPIO_OUTPUT_PIN_SEL  	((1ULL<<LED_1) | (1ULL<<LED_2))
#define GPIO_INPUT_PIN_SEL  	(1ULL<<BUTTON)

/* Protótipos de Funções */
void app_main( void );
void task_GPIO_Blink( void *pvParameter );
static void IRAM_ATTR gpio_isr_handler( void *arg );

/* Variáveis Globais */
static const char * TAG = "main: ";
const char * msg[2] = {"Ligado", "Desligado"};
volatile int contador=0; 
 
void task_GPIO_Blink( void *pvParameter )
{
	 /*  Parâmetros de controle da GPIO da função "gpio_set_direction"
		GPIO_MODE_OUTPUT       			//Saída
		GPIO_MODE_INPUT        			//Entrada
		GPIO_MODE_INPUT_OUTPUT 			//Dreno Aberto
    */
	gpio_set_direction( LED_BUILDING, GPIO_MODE_OUTPUT );
	gpio_set_level( LED_BUILDING, 1 );  //O Led desliga em nível 1;		
	bool estado = 0; 
	
    while ( TRUE ) 
    {		
		estado = !estado;
        if( DEBUG )
            ESP_LOGI(TAG, "Led Building: %s", msg[estado] );
        gpio_set_level( LED_BUILDING, estado ); 				
		
        vTaskDelay( 2000 / portTICK_RATE_MS ); //Delay de 2000ms liberando scheduler;
	}
}	

/* ISR (função de callback) esta função de IRQ será chamada sempre que acontecer uma interrupção. */
static void IRAM_ATTR gpio_isr_handler( void* arg )
{	
	//Verifica qual botão ativou a interrupção.
	if( BUTTON == (uint32_t) arg )
	{			
		gpio_set_level(LED_1,contador%2);				
	}
	contador++;	
}

/* Aplicação Principal (Inicia após bootloader) */
void app_main( void )
{	
	/*  Parâmetros de controle da GPIO da função "gpio_set_direction"
		GPIO_MODE_OUTPUT       			//Saída
		GPIO_MODE_INPUT        			//Entrada
		GPIO_MODE_INPUT_OUTPUT 			//Dreno Aberto
    */	
	gpio_config_t output_conf = {
		.intr_type = GPIO_INTR_DISABLE, //Desabilita interrupção externa.
		.mode = GPIO_MODE_OUTPUT, //Configura GPIO como saídas.
		.pin_bit_mask = GPIO_OUTPUT_PIN_SEL //Carrega GPIO configuradas.
	};
    gpio_config( &output_conf );  //Configura GPIO conforme descritor.
    
	/*	Parâmetros de controle da resistência interna na função "gpio_set_pull_mode"
		GPIO_PULLUP_ONLY,               // Pad pull up            
		GPIO_PULLDOWN_ONLY,             // Pad pull down          
		GPIO_PULLUP_PULLDOWN,           // Pad pull up + pull down
		GPIO_FLOATING,                  // Pad floating  
	*/
	/*
		GPIO_INTR_DISABLE = 0,     		// Desabilita interrupção da GPIO                            
		GPIO_INTR_POSEDGE = 1,     		// Habilita interrupção na borda de subida                  
		GPIO_INTR_NEGEDGE = 2,     		// Habilita interrupção na borda de descida               
		GPIO_INTR_ANYEDGE = 3,     		// Habilita interrupção em ambas as borda 
	*/
	gpio_config_t input_conf = {
		.intr_type = GPIO_INTR_NEGEDGE,  //Habilita interrupção na borda de descida 
		.mode = GPIO_MODE_INPUT, //Configura GPIO como entradas.
		.pin_bit_mask = GPIO_INPUT_PIN_SEL, //Carrega GPIO configuradas.
		.pull_down_en = GPIO_PULLDOWN_DISABLE, //Desabilita Pull-down das GPIO's.
		.pull_up_en = GPIO_PULLUP_ENABLE //Habilita Pull-up das GPIO's.
    };
	gpio_config(&input_conf);    //Configura GPIO conforme descritor.

	//Habilita a interrupção externa da(s) GPIO's. 
	//Ao utilizar a função gpio_install_isr_service todas as interrupções de GPIO do descritor vão chamar a mesma 
	//interrupção. A função de callback que será chamada para cada interrupção é definida em gpio_isr_handler_add. 
	gpio_install_isr_service(0);

	//Registra a interrupção externa do BUTTON
    gpio_isr_handler_add( BUTTON, gpio_isr_handler, (void*) BUTTON ); 	

	// Cria a task responsável pelo blink LED. 
	if( (xTaskCreate( task_GPIO_Blink, "task_GPIO_Blink", 2048, NULL, 1, NULL )) != pdTRUE )
    {
      if( DEBUG )
        ESP_LOGI( TAG, "error - Nao foi possivel alocar task_GPIO_Blink.\r\n" );  
      return;   
    }	
}