/*
 * The MIT License
 *
 * Copyright 2024 Alvaro Salazar <alvaro@denkitronik.com>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <libiot.h>
#include <libwifi.h>
#include <stdio.h>

/*********** Inicio de parametros configurables por el usuario *********/

// Variables de entorno - se configuran en platformio.ini o .env
// Los topicos deben tener la estructura: <país>/<estado>/<ciudad>/<usuario>/out
#ifndef COUNTRY
#define COUNTRY "colombia"                        ///< País (definir vía .env)
#endif
#ifndef STATE
#define STATE "valle"                           ///< Estado/Departamento (definir vía .env)
#endif
#ifndef CITY
#define CITY "tulua"                            ///< Ciudad (definir vía .env)
#endif
#ifndef MQTT_SERVER
#define MQTT_SERVER "rivasmqtt.freeddns.org"                     ///< Servidor MQTT (definir vía .env)
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 8883                            ///< Puerto seguro (TLS)
#endif
#ifndef MQTT_USER
#define MQTT_USER "admin"                         ///< Usuario MQTT (definir vía .env)
#endif
#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD "admin123"                    ///< Contraseña MQTT (definir vía .env)
#endif

// Variables de configuración de la red WiFi
#ifndef WIFI_SSID
#define WIFI_SSID ""                       ///< SSID por defecto vacío; usar aprovisionamiento
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""                   ///< Password por defecto vacío; usar aprovisionamiento
#endif

// Alias para compatibilidad con el código existente
#define SSID WIFI_SSID
#define PASSWORD WIFI_PASSWORD

// Certificado raíz - se configura como variable de entorno
#ifndef ROOT_CA
#define ROOT_CA "-----BEGIN CERTIFICATE-----\nMIIEVjCCAj6gAwIBAgIQY5WTY8JOcIJxWRi/w9ftVjANBgkqhkiG9w0BAQsFADBP\nMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFy\nY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMTAeFw0yNDAzMTMwMDAwMDBa\nFw0yNzAzMTIyMzU5NTlaMDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBF\nbmNyeXB0MQswCQYDVQQDEwJFODB2MBAGByqGSM49AgEGBSuBBAAiA2IABNFl8l7c\nS7QMApzSsvru6WyrOq44ofTUOTIzxULUzDMMNMchIJBwXOhiLxxxs0LXeb5GDcHb\nR6EToMffgSZjO9SNHfY9gjMy9vQr5/WWOrQTZxh7az6NSNnq3u2ubT6HTKOB+DCB\n9TAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMB\nMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFI8NE6L2Ln7RUGwzGDhdWY4j\ncpHKMB8GA1UdIwQYMBaAFHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEB\nBCYwJDAiBggrBgEFBQcwAoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzATBgNVHSAE\nDDAKMAgGBmeBDAECATAnBgNVHR8EIDAeMBygGqAYhhZodHRwOi8veDEuYy5sZW5j\nci5vcmcvMA0GCSqGSIb3DQEBCwUAA4ICAQBnE0hGINKsCYWi0Xx1ygxD5qihEjZ0\nRI3tTZz1wuATH3ZwYPIp97kWEayanD1j0cDhIYzy4CkDo2jB8D5t0a6zZWzlr98d\nAQFNh8uKJkIHdLShy+nUyeZxc5bNeMp1Lu0gSzE4McqfmNMvIpeiwWSYO9w82Ob8\notvXcO2JUYi3svHIWRm3+707DUbL51XMcY2iZdlCq4Wa9nbuk3WTU4gr6LY8MzVA\naDQG2+4U3eJ6qUF10bBnR1uuVyDYs9RhrwucRVnfuDj29CMLTsplM5f5wSV5hUpm\nUwp/vV7M4w4aGunt74koX71n4EdagCsL/Yk5+mAQU0+tue0JOfAV/R6t1k+Xk9s2\nHMQFeoxppfzAVC04FdG9M+AC2JWxmFSt6BCuh3CEey3fE52Qrj9YM75rtvIjsm/1\nHl+u//Wqxnu1ZQ4jpa+VpuZiGOlWrqSP9eogdOhCGisnyewWJwRQOqK16wiGyZeR\nxs/Bekw65vwSIaVkBruPiTfMOo0Zh4gVa8/qJgMbJbyrwwG97z/PRgmLKCDl8z3d\ntA0Z7qq7fta0Gl24uyuB05dqI5J1LvAzKuWdIjT1tP8qCoxSE/xpix8hX2dt3h+/\njujUgFPFZ0EVZ0xSyBNRF3MboGZnYXFUxpNjTWPKpagDHJQmqrAcDmWJnMsFY3jS\nu1igv3OefnWjSQ==\n-----END CERTIFICATE-----"                       ///< CA vacía por defecto; definir vía .env
#endif

const char* root_ca = ROOT_CA;

/*********** Fin de parametros configurables por el usuario ***********/


/* Constantes de configuración del servidor MQTT, no cambiar */
const char* mqtt_server = MQTT_SERVER;            ///< Dirección de tu servidor MQTT
const int mqtt_port = MQTT_PORT;                  ///< Puerto seguro (TLS)
const char* mqtt_user = MQTT_USER;                ///< Usuario MQTT
const char* mqtt_password = MQTT_PASSWORD;        ///< Contraseña MQTT

// Obtener la MAC Address
String macAddress = getMacAddress();
const char * client_id = macAddress.c_str();      ///< ID del cliente MQTT

// Tópicos de publicación y suscripción
String mqtt_topic_pub( String(COUNTRY) + "/" + String(STATE) + "/"+ String(CITY) + "/" + String(client_id) + "/" + String(mqtt_user) + "/out");
String mqtt_topic_sub( String(COUNTRY) + "/" + String(STATE) + "/"+ String(CITY) + "/" + String(client_id) + "/" + String(mqtt_user) + "/in");

// Convertir los tópicos a constantes de tipo char*
const char * MQTT_TOPIC_PUB = mqtt_topic_pub.c_str();
const char * MQTT_TOPIC_SUB = mqtt_topic_sub.c_str();

long long int measureTime = millis();   // Tiempo de la última medición
long long int alertTime = millis();     // Tiempo en que inició la última alerta
WiFiClientSecure espClient;             // Conexión TLS/SSL con el servidor MQTT
PubSubClient client(espClient);         // Cliente MQTT para la conexión con el servidor
time_t now;                             // Timestamp de la fecha actual.
const char* ssid = SSID;                // Cambia por el nombre de tu red WiFi
const char* password = PASSWORD;        // Cambia por la contraseña de tu red WiFi