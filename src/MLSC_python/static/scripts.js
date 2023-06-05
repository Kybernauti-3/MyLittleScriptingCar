
var direction = [0, 0]
var us

function sendkeys(connection, keys){
    direction = [0, 0]

    direction[1] += ("87" in keys) * 120;  // w
    direction[1] -= ("83" in keys) * 120;  // s
    direction[0] -= ("65" in keys) * 120;  // a
    direction[0] += ("68" in keys) * 120;  // d

    // Do our thing
    connection.send("sv:" + direction);
}

function sendslid(connection, id1, id2){
    var rot = document.getElementById(id1);
    var vel = document.getElementById(id2);

    direction = [rot.value, vel.value];

    // Do our thing
    connection.send("sv:" + direction);
}

function uschange(connection, checkbox){
    if (checkbox.checked) {
        connection.send("su:1");
        us = setInterval(() => { connection.send("gu") }, 500);
    } else {
        connection.send("su:0");
        clearInterval(us);
    }
}
