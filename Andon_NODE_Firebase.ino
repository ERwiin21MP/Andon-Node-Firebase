#include <NTPClient.h>            /* Esta librería permite utilizar un cliente NTP (Network Time Protocol) en el dispositivo. El NTP se utiliza para sincronizar el reloj del dispositivo con una fuente de tiempo en la red, como un servidor NTP, para asegurar que el dispositivo tenga la hora correcta. */
#include <WiFiUdp.h>              /* Esta librería proporciona funcionalidad para establecer una conexión UDP (User Datagram Protocol) utilizando WiFi. Se utiliza para comunicarse a través de UDP en una red inalámbrica. */
#include <FirebaseESP8266.h>      /* Esta librería proporciona una interfaz para interactuar con la plataforma Firebase utilizando un módulo ESP8266. Firebase es una plataforma de desarrollo de aplicaciones móviles y web que ofrece una variedad de servicios, como almacenamiento en la nube, bases de datos en tiempo real y autenticación de usuarios. */
#include <addons/TokenHelper.h>   /* Esta librería es un complemento de la librería FirebaseESP8266 y se utiliza para gestionar y obtener tokens de autenticación necesarios para realizar operaciones en la plataforma Firebase. */

/* Definición de las constantes de conexión WiFi y Firebase */
#define WIFI_SSID "EFAMS"                                         /* Es el nombre de la red WiFi a la que el dispositivo se conectará. */
#define WIFI_PASSWORD "TX4TNsgsp9SGh4aF"                          /* Es la contraseña de la red WiFi. */
#define API_KEY "AIzaSyDY7mWuQIZ1ln9xA3NvniUOM9EYTSX3DiE"         /* Es la clave de API de Firebase, que se utiliza para autenticar las solicitudes a la plataforma Firebase. */
#define DATABASE_URL "andon-richter-default-rtdb.firebaseio.com"  /* Es la URL de la base de datos de Firebase donde se almacenarán los datos. */
#define USER_EMAIL "andon_user@gmail.com"                         /* Es el correo electrónico del usuario de Firebase, utilizado para la autenticación. */
#define USER_PASSWORD "12345678"                                  /* Es la contraseña del usuario de Firebase. */

/* Definición de las constantes para los pines de los LEDs */
const int led1 = D0;
const int led2 = D1;
const int led3 = D2;
const int led4 = D3;

/* Variables para almacenar el estado anterior y actual de los LEDs */
int estadoAnterior1 = LOW;
int estadoAnterior2 = LOW;
int estadoAnterior3 = LOW;
int estadoAnterior4 = LOW;
int estadoActual1;
int estadoActual2;
int estadoActual3;
int estadoActual4;

/* Variables para almacenar la información que se guardara en la base de datos */

int tiempo_de_resolucion_horas = 0;     /* Horas de resolución */
int tiempo_de_resolucion_minutos = 0;   /* Minutos de resolución */
String estado = "";                     /* Estado del led */
String responsable = "";                /* Responsable*/
String accion = "";                     /* Acción tomada en la resolución */
String descripcion = "";                /* Descripción del evento */

FirebaseData fbdo;                      /* Objeto para manejar las operaciones de Firebase */
FirebaseAuth auth;                      /* Objeto para la autenticación en Firebase */
FirebaseConfig config;                  /* Objeto para la configuración de Firebase */
FirebaseJson json;                      /* Objeto para manejar datos JSON en Firebase */
WiFiUDP ntpUDP;                         /* Objeto para la conexión UDP para el cliente NTP */
NTPClient timeClient(ntpUDP, "pool.ntp.org", -6 * 3600, 60000);   /* Cliente NTP para obtener la hora */

void setup(){
  Serial.begin(9600);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT); 
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a Wi-Fi");

  /* Segmento de codigo para conectar al WiFi y visualizar el estado en los leds
   *  1. Entra al while y intenta conectar al WiFi, los leds parpadean para indicar el intento de conexión
   *  2. Sale del while cuando ya se conecto al WiFi
   *  3. Enciende los led en forma 1000 1100 1110 1111 1110 1100 1000 0000, esto indica que ya se conecto
   *  4. Configura las entradas 
   *  5. Configura la base de datos
   *  6. Imprime informacion sobre la base de datos
  */
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(led1, 1);
    digitalWrite(led2, 1);
    digitalWrite(led3, 1);
    digitalWrite(led4, 1);
    delay(300);
    digitalWrite(led1, 0);
    digitalWrite(led2, 0);
    digitalWrite(led3, 0);
    digitalWrite(led4, 0);
    delay(300);
  }
  int Tiempo = 200;
  digitalWrite(led1, 1);
  delay(Tiempo);
  digitalWrite(led2, 1);
  delay(Tiempo);
  digitalWrite(led3, 1);
  delay(Tiempo);
  digitalWrite(led4, 1);
  delay(Tiempo);
  digitalWrite(led4, 0);
  delay(Tiempo);
  digitalWrite(led3, 0);
  delay(Tiempo);
  digitalWrite(led2, 0);
  delay(Tiempo);
  digitalWrite(led1, 0);
  pinMode(led1, INPUT);
  pinMode(led2, INPUT); 
  pinMode(led3, INPUT);
  pinMode(led4, INPUT);
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  timeClient.begin();                 /* Inicia la conexión del cliente NTP para sincronizar el tiempo */
}

void loop(){
  /* Lee el estado actual de los pines de los LED */
  estadoActual1 = digitalRead(led1);
  estadoActual2 = digitalRead(led2);
  estadoActual3 = digitalRead(led3);
  estadoActual4 = digitalRead(led4);

  if (estadoActual1 != estadoAnterior1) { /* Entra al if si se ha detectado un cambio en el led */
    if (estadoActual1 == HIGH) { /* Si es es verdadero: el led cambio a estado alto, si es falso: el led cambio a estado bajo */
      /* Se establece la informacion para subir a la base de datos */
      estado = "Rojo";
      responsable = "Operador";
      descripcion = "Se presiono el boton rojo y el led cambio a estado alto";
      Serial.println("El pin 1 cambió a estado alto");
      push();
    } else {
      estado = "Ninguno";
      responsable = "Operador";
      descripcion = "Se presiono el boton rojo y el led cambio a estado bajo";
      Serial.println("El pin 1 cambió a estado bajo");
      push();
    }
    estadoAnterior1 = estadoActual1;
  }

  if (estadoActual2 != estadoAnterior2) {
    if (estadoActual2 == HIGH) {
      estado = "Verde";
      responsable = "Tecnico en procesos / Supervisor";
      descripcion = "Se presiono el boton verde y el led cambió a estado alto";
      Serial.println("El pin 2 cambió a estado alto");
      push();
    } else {
      estado = "Ninguno";
      responsable = "Tecnico en procesos / Supervisor";
      descripcion = "Se presiono el boton verde y el led cambió a estado bajo";
      Serial.println("El pin 2 cambió a estado bajo");
      push();
    }
    estadoAnterior2 = estadoActual2;
  }

  if (estadoActual3 != estadoAnterior3) {
    if (estadoActual3 == HIGH) {
      estado = "Azul";
      responsable = "Coordinador";
      descripcion = "Se presiono el boton azul y el led cambió a estado alto";
      Serial.println("El pin 3 cambió a estado alto");
      push();
    } else {
      estado = "Ninguno";
      responsable = "Coordinador";
      descripcion = "Se presiono el boton azul y el led cambió a estado bajo";
      Serial.println("El pin 3 cambió a estado bajo");
      push();
    }
    estadoAnterior3 = estadoActual3;
  }

  if (estadoActual4 != estadoAnterior4) {
    if (estadoActual4 == HIGH) {
      estado = "Amarillo";
      responsable = "Lider de calidad";
      descripcion = "Se presiono el boton amarillo y el led cambió a estado alto";
      Serial.println("El pin 4 cambió a estado alto");
      push();
    } else {
      estado = "Ninguno";
      responsable = "Lider de calidad";
      descripcion = "Se presiono el boton amarillo y el led cambió a estado bajo";
      Serial.println("El pin 4 cambió a estado bajo");
      push();
    }
    estadoAnterior4 = estadoActual4;
  }
  delay(10); // Pequeña pausa para estabilizar el cambio de estado del pin
}

/* Función para enviar los datos al servidor Firebase */
void push() {
  /* Actualizar la hora del cliente NTP */
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  /* Agregar los datos al objeto JSON */
  json.add("numero_de_maquina", 1);
  json.add("nombre_de_la_maquina", "Alpha Machine");
  json.add("dia", ptm->tm_mday);
  json.add("mes", ptm->tm_mon + 1);
  json.add("año", ptm->tm_year + 1900);
  json.add("hora", timeClient.getHours());
  json.add("minuto", timeClient.getMinutes());
  json.add("segundo", timeClient.getSeconds());
  json.add("responsable", responsable);
  json.add("tiempo_de_resolucion_horas", 0);
  json.add("tiempo_de_resolucion_minutos", 0);
  json.add("accion", "Aqui va la acción que se tomó para resolver el problema");
  json.add("descripcion", descripcion);
  json.add("estado", estado);

  /* Enviar los datos al servidor Firebase */
  Firebase.push(fbdo, "registros_andon/Maquina_1/", json);
  json.add("id", fbdo.pushName());

  Firebase.updateNode(fbdo, "registros_andon/Maquina_1/" + fbdo.pushName(), json);
}
