
direction = [0, 0]
var us

function sendkeys(connection, keys){
    direction = [0, 0]
    
    direction[1] += ("87" in keys) * 100;  // w
    direction[1] -= ("83" in keys) * 100;  // s
    direction[0] -= ("65" in keys) * 100;  // a
    direction[0] += ("68" in keys) * 100;  // d

    // Do our thing
    connection.send(direction)
}

function uschange(connection){
    checkbox = document.getElementById('uschange')
    if (checkbox.checked) {
        connection.send("su1");
        us = setInterval(() => { connection.send("ug") }, 500);
    } else {
        connection.send("su0");
        clearInterval(us)
    }
}

function senddata(connection, pre, post){
    
}
