function(doc) {
 if(doc.readings.lat) {
    emit([doc.readings.lat,doc.readings.lon], doc);
  }
}
