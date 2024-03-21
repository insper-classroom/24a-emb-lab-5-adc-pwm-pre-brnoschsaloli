#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int lista[5] = {0};
    int i = 0;
    float soma = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, portMAX_DELAY)) {
                soma -= lista[i]; // Subtrai o valor mais antigo da soma
                lista[i] = data; // Atualiza o buffer com o novo valor
                soma += data; // Adiciona o novo valor à soma

                if (++i >= 5) {
                    i = 0; // Volta ao início do buffer se alcançar o final
                }

                int media = soma / 5; // Calcula a média
                printf("Média móvel: %d\n", media);
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
