db = require('../../index')('data');

db.open('put_benchmark');

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
    db.put(idx,idx);
    if (count % 5000 == 0) {
        console.log(count, 'done');
        return setImmediate(_batch.bind(null,batch, count, tme))
    } else {
      return _batch(batch, count, tme);
    }

  }

  time('PUT', tme, new Date());
}


_batch(db, batch_count, new Date());
