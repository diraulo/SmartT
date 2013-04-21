function(doc, req) {
   if (req.method !== 'POST') {
	  return [null, 'only POST allowed'];
   }
   if(!doc) {
        doc = {} 
        if(req.id){
              doc._id = req.id
        } else {
              doc._id = req.uuid
        }
    }

   doc.timestamp = new Date();
   doc.readings = JSON.parse(req.body)
   doc.userid = req.query.userid

   return [doc, JSON.stringify(doc)];
}
