function(doc) {
 if(doc.readings.id) {
    emit([doc.readings.id,doc.timestamp], doc);
  }
}
