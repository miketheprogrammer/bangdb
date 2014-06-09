db = require('../../index').embedded('data');
db.open('scans');


function time(name, start, end, suffix) {
    suffix = suffix || 'ms'
    console.log(name, 'took', end - start, suffix);
}


var transaction = db.transaction();
var start = new Date();

for (var i = 0; i < (parseInt(process.argv[2]) || 1000); i += 1) {
    var idx = i.toString();
    transaction.put(idx,idx);
}
time("TRANSACTION::PUT - "+process.argv[2], start, new Date());
start = new Date();
transaction.commit();

time("TRANSACTION::WRITE", start, new Date());

start = new Date();

var scan = db.scan('0', '200', {skey_op: 2, ekey_op: 1, limit_by: 1, limit: 10});
while(scan.hasNext()) {
    console.log(scan.next());
}

time("GET", start, new Date());