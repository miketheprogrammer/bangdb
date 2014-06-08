db = require('../../index')('data');
db.open('transactions');


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
time("TRANSACTION::PUT"+process.argv[2], start, new Date());
start = new Date();
transaction.commit();

time("TRANSACTION::WRITE", start, new Date());

for (var i = 0; i < (parseInt(process.argv[2]) || 1000); i += 1) {
    var idx = i.toString();
}
