db = require('../../index')('data');

db.open('batch_benchmark');

batch = db.batch();


batch_count = 10000;

function time(name, start, end) {
  console.log(name, 'took', end - start, 'ms');
}


var idx;
function _batch(batch, count, tme) {
  idx = count.toString();
  
  if (count > 0) {
    count -= 1;
    batch.put(idx,idx);
    if (count % 1000 == 0) {
        console.log(count, 'done');
        time("BATCH", tme, new Date());
        batch.write();
        batch = db.batch();
        return setImmediate(_batch.bind(null,batch, count, tme))
    } else {
      return _batch(batch, count, tme);
    }

  }
  console.log('Writing batch');
  batch.write();

  time('BATCH', tme, new Date());
}


_batch(batch, batch_count, new Date());
