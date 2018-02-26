const { FileMapping, Mutex } = require('./build/Release/addon');

map = new FileMapping();
lock = new Mutex();

async function waitFor(ms) {
  return new Promise(function (resolve, reject) {
    setTimeout(function() {
      return resolve();
    }, ms);
  });
}

function main() {
  return new Promise(async function(resolve, reject) {
    buffer = Buffer.alloc(4) // 4 byte buffer to write to a 4 byte shared storage
    map.openMapping('howard_mem_map', 4)
    lock.open('howard_mem_map_lock');

    for (var i = 0; i < 60; ++i)
    {
      // Wait 1 second between changes
      await waitFor(10);

      // Write the new value
      buffer.writeInt32LE(i);

      lock.wait();
      map.writeBuffer(buffer, 0, 0, 4);
      lock.release();

      console.log('Wrote ' + i);
    }

    map.closeMapping()
    lock.close();
    resolve();
  })
}


mainPromise = main()
mainPromise.then(function() {
  console.log('End!')
});
