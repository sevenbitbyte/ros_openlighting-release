#include "olamanager.h"

#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/StreamingClient.h>

OlaManager::OlaManager(/*QObject *parent*/) /*:
    QObject(parent)*/
{

    // turn on OLA logging
    ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);

    _client = new ola::StreamingClient();

    // Setup the client, this connects to the OLA server
    if (!_client->Setup()) {
        qDebug() << "ERROR: OLA Setup failed" << endl;
        return;
    }

}

OlaManager::~OlaManager() {
    //
}

void OlaManager::sendBuffers(){
    QMap<int,ola::DmxBuffer*>::const_iterator iter = _buffers.constBegin();
    for(; iter != _buffers.constEnd(); iter++ ){
        _client->SendDmx(iter.key(), *iter.value());
    }

    /*QList<int> universes = _buffers.keys();

    for(int i=0; i<universes.size(); i++){

        int universe = universes[i];

        _client->SendDmx(universe, *_buffers[universe]);

    }*/
}

void OlaManager::blackout(){
    QList<int> universes = _buffers.keys();

    for(int i=0; i<universes.size(); i++){
        int universe = universes[i];
        _buffers[universe]->Blackout();
    }
}

void OlaManager::lightsOn(int value){
    QList<int> universes = _buffers.keys();

    for(int i=0; i<universes.size(); i++){
        int universe = universes[i];
        _buffers[universe]->SetRangeToValue(0, value, 512);
    }
}

void OlaManager::createBuffer(int universe) {
    if( !_buffers.contains(universe) ){
        ola::DmxBuffer* buffer = new ola::DmxBuffer();
        buffer->Blackout();

        _buffers.insert(universe, buffer);
    }
}

ola::DmxBuffer* OlaManager::getBuffer(int universe){
    if(_buffers.contains(universe)){
        return _buffers[universe];
    }

    ola::DmxBuffer* buffer = new ola::DmxBuffer;

    _buffers.insert(universe, buffer);

    return buffer;
}

void OlaManager::setPixel(DmxAddress address, QColor color){
    ola::DmxBuffer* buffer = getBuffer(address.universe);

    uint8_t colorData[3] = {(uint8_t)color.red(), (uint8_t)color.green(), (uint8_t)color.blue()};

    buffer->SetRange(address.offset, colorData, 3);
}

void OlaManager::updateBuffers(QMap<int,ola::DmxBuffer> data){
    QList<int> universes = data.keys();

    for(int i=0; i<data.size(); i++){
        int universe = universes[i];

        updateBuffer(universe, data[universe]);
    }
}


void OlaManager::updateBuffer(int universe, ola::DmxBuffer& data){
    ola::DmxBuffer* buffer = new ola::DmxBuffer(data);

    if( !_buffers.contains(universe) ){
        _buffers.insert( universe, buffer);
        return;
    }

    _buffers[universe] = buffer;
}


