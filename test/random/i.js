db = require('../../index')('data');

db.open('batch_put_benchmark');



batch_count = 1000;

function time(name, start, end) {
  console.log(name, 'took', end - start, 'ms');
}

var _batch = [];
var idx;
var tme = new Date();
for (var i = 0; i < batch_count; i += 1) {
    var idx = i.toString();
    _batch.push({type:'put', key:idx, value:idx});
}

console.log('batching');
db.batch(_batch, function(err, res) {
    time('Batch:', tme, new Date());
});
