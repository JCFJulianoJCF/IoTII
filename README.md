# IoT II - Exemplos de Códigos

Bem vindos ao repositório de exemplos da disciplina de IoT II. 
Estes exemplos foram testados com a versão 3.3 do RTOS-SDK da Espressif para ESP8266.

- ***EX01_GPIO***: Neste exemplo, uma saída digital se inverte a cada 2 segundos e uma entrada digital controla o estado de outras duas saídas digitais. 
- ***EX02_GPIOTask***: Este exemplo utiliza da criação de 2 tasks para executar separadamente as funções de blink e leitura digital.
- ***EX03_GPIODescritor***: Quando se tem muitos pinos a serem configurados podemos usar uma variável descritora. Este exemplo demonstra como configurar os pinos de GPIO através de um descritor.
- ***EX04_GPIOInterrupt***: Os pinos de entrada podem ser configurados como interrupção externa possibilitando sincronismo na execução de tarefas. Neste exemplo é apresentado como utilizar o vetor de interrupção externa e também uma maneira mais elegante de trabalhar com as variáveis descritoras.
- ***EX05_WiFiIPDinamico***: Este exemplo demonstra os primeiros passos para configuração do módulo WiFi implementando um eventGroup para sincronizar uma tarefa que escreve o IP atribuido no terminal. Neste exemplo o IP do ESP é atribuido automaticamente pelo roteador.
- ***EX05_WiFiIPEstatico***: Este é uma cópia do exemplo anterior apenas incluindo comandos para configurar o WiFi com IP fixo.
