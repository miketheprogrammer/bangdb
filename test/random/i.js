db = require('../../index')('data');

db.open('batch_put_benchmark');



batch_count = 200;

function time(name, start, end) {
  console.log(name, 'took', end - start, 'ms');
}

var _batch = [];
var idx;

for (var i = 0; i < batch_count; i += 1) {
    var idx = i.toString();
    _batch.push({type:'put', key:idx, value:idx});
}
var tme = new Date();
console.log('batching');
db.batch(_batch, function(err, res) {
    time('Batch:', tme, new Date());
});