/*
 * 
 * void eolterm() = end of line seperater
 * void connectToWifi()
 * connectToMqtt()
 * WiFiEvent(WiFiEvent_t event)
 * onMqttDisconnect
 * onMqttSubscribe
 * onMqttUnsubscribe
 * onMqttMessage
 * interbmsstatus
 * interbmsstatusHA
 * onMqttPublish 
 * sendMQTTData1    = main daly data eg percentage
 * sendMqttCells   = cell voltages
 * sendMQTTDataAlarm1
 * sendMQTTDataAlarm2
 * sendMQTTDatarx() = send a message in response to data received from the cmd message
 * sendMQTTDatacellbal() = cell balancing
 * onMqttConnect = configure the rx channel
 * bmsPriority()  =not in use
 * sendCanTXFrame = final send of messages
 * checkForRX()  = canbus rx not in use possbily
 * leds control = used to flash leds during startup
 * oledMessage = display refresh
 * getEverythingFromDaly() = update bms values to floats etc
 * getEverythingFromDalyCells() = individual cell voltages
 * getEverythingFromDalyAlarm() = filter to consolidate all alarms
 * getEverythingFromDalyorig() = original void for refernece
 * sendmqttcanbusvalues() = send a copy of actual canbus values to HA
 * batteryStartSync() = not in use
 * batterylogic() =  control to decide which values are used in canbus sibgle or dual, also canbus priority
 * updateTXFrames() = update the frame values pre transmission
 * updateTXFramesorig() = original for reference
 * mqttMessSelect = which message to be sent
 * 
 */
