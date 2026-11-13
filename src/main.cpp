
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

const String enderecoApi = "https://brasilapi.com.br/api/cep/v2/";

String consultaApiCep(String cep);

void conexaoWiFi();
void reconexaoWiFi();

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

const unsigned long TempoConexao = 10000;
const unsigned long TempoReconexao = 5000;

unsigned long tempoUltimaconexao = 0;

void setup()
{
  Serial.begin(115200);

  conexaoWiFi();

  Serial.println("Digite seu CEP:");
}

void loop()
{
  reconexaoWiFi();

  if (Serial.available())
  {
    String CEP = Serial.readString();
    CEP.trim();

    if (CEP.length() == 8)
    {
      bool valido = true;

      for (int i = 0; i < CEP.length(); i++)
      {
        if (!isDigit(CEP[i]))
        {
          valido = false;
        }
      }

      if (valido)
      {
        if (WiFi.status() == WL_CONNECTED)
        {
          String respostaApiCep = consultaApiCep(CEP);

          if (respostaApiCep != "")
          {
            JsonDocument doc;

            DeserializationError erro = deserializeJson(doc, respostaApiCep);

            if (!erro)
            {
              String rua = doc["street"].as<String>();
              String bairro = doc["neighborhood"].as<String>();
              String cidade = doc["city"].as<String>();
              String estado = doc["state"].as<String>();

              Serial.println();
              Serial.println("DADOS DO ENDERECO");
              Serial.println("-----------------");
              Serial.println("CEP: " + CEP);
              Serial.println("Rua: " + rua);
              Serial.println("Bairro: " + bairro);
              Serial.println("Cidade: " + cidade);
              Serial.println("Estado: " + estado);
            }

            if (erro)
            {
              Serial.println("Erro ao interpretar o JSON");
            }
          }
        }
      }

      if (!valido)
      {
        Serial.println("CEP invalido!");
      }
    }

    if (CEP.length() != 8)
    {
      Serial.println("CEP invalido! Digite 8 digitos.");
    }

    Serial.println("\nDigite outro CEP:");
  }
}

void conexaoWiFi()
{
  Serial.printf("Conectando-se a rede Wi-Fi: %s", SSID);
  WiFi.begin(SSID, SENHA);

  unsigned long tempoWiFi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWiFi < TempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWi-Fi funcionando!");
    Serial.print("Endereco IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nWi-Fi nao funcionando.");
  }
}

void reconexaoWiFi()
{
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoUltimaconexao >= TempoReconexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao Wi-Fi perdida!");
      conexaoWiFi();
    }

    tempoUltimaconexao = tempoAtual;
  }
}

String consultaApiCep(String cep)
{
  HTTPClient http;
  http.begin(enderecoApi + cep);

  int codigoHttp = http.GET();

  String resposta;

  if (codigoHttp == 200)
  {
    resposta = http.getString();
  }

  if (codigoHttp == 404)
  {
    Serial.println("CEP nao encontrado!");
  }

  if (codigoHttp != 200 && codigoHttp != 404)
  {
    Serial.println("Erro. Codigo HTTP: " + String(codigoHttp));
  }

  http.end();

  return resposta;
}
