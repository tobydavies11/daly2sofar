void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  eolterm();
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
  eolterm();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      WiFiStatus = true;
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      WiFiStatus = false;
      //        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      //        xTimerStart(wifiReconnectTimer, 0);
      connectToWifi(); // added to ensure it reconnects during a drop out
      break;
  }
      eolterm();
}
//=======================================================
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("VOID:Disconnected from MQTT.");
  //  xTimerStop(mqttPublishXTimer, 0);
  MQTTStatus = false;
  delay (200); // slow it down
  //mqttPublisherTimer->Stop();
  if (WiFi.isConnected()) {
    //    xTimerStart(mqttReconnectTimer, 0);
    connectToMqtt();
  }
    eolterm();
}
//======================================
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("VOID:Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
      eolterm();
}
//======================================
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("VOID:Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
      eolterm();
}
//=======================================
void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  eolterm();
}
//==============================================================   
