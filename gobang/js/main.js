var clientWidth = document.documentElement.clientWidth;  //屏幕宽度
var clientHeight = document.documentElement.clientHeight;   //屏幕高度
console.log(clientWidth + "X" + clientHeight);

/*定义一个二维数组a[15][15]，保存当前棋盘状态*/
var curCbArray = arrayInit(15);

/*定义一个二维数组，保存前一次的棋盘状态*/
var lastCbArray = arrayInit(15);

/*定义一个二维数组，保存前两次的棋盘状态*/
var llastCbArray = arrayInit(15);

/*棋子初始化*/
var black = new Image();
var white = new Image();
black.src = "images/black.png";
white.src = "images/white.png";

/*canvas初始化*/
var canvasOfMain = document.getElementById('chessboard');
var ctxOfMain = canvasOfMain.getContext("2d");  //获取该canvas的2D绘图环境对象
ctxOfMain.strokeStyle = "#333";
drawChessboard(ctxOfMain);

/*棋盘状态标志位*/
var isBlack = true; //当前棋子是否为黑色
var isGameOver = false; //当前局是否结束
var hasPiece = 0; //当前棋盘有几颗棋子
var revokeFlag = 0;    //悔棋标志位，大于等于2才能悔棋

/*状态栏初始化*/
var canvasOfStatusBar = document.getElementsByClassName('statusbar');
var ctxOfStatusBar = canvasOfStatusBar[0].getContext("2d");

ctxOfStatusBar.beginPath();
ctxOfStatusBar.font = ("100px Georgia");
ctxOfStatusBar.fillStyle = "#F70707";

/*二维数组初始化*/
function arrayInit(size) {
    var arr = new Array(size);
    for (var i = 0; i < size; i++) {
        arr[i] = new Array(size);
        for (var j = 0; j < size; j++) {
            arr[i][j] = 0;
        }
    }
    return arr;
}

/*二维数组间复制*/
function arrayCopy(dst, src) {
    for (var i = 0; i < 15; i++) {
        for (var j = 0; j < 15; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

/*二维数组清空*/
function arrayReset(src) {
    for (var i = 0; i < 15; i++) {
        for (var j = 0; j < 15; j++) {
            src[i][j] = 0;
        }
    }
}

/*绘制棋盘*/
function drawChessboard(ctxOfMain) {
    //下面这四行绘制正方形，对于15X15的棋盘，横竖绘制14个正方形即可
    for (var i = 0; i < curCbArray.length - 1; i++) {
        for (var j = 0; j < curCbArray.length - 1; j++) {
            ctxOfMain.strokeRect(i * 36 + 36, j * 36 + 36, 36, 36);  //绘制40像素的正方形
        }
    }

    /*绘制棋盘上的五个点*/
    ctxOfMain.fillRect(3 * 36 + 33, 3 * 36 + 33, 6, 6);
    ctxOfMain.fillRect(3 * 36 + 33, 11 * 36 + 33, 6, 6);
    ctxOfMain.fillRect(11 * 36 + 33, 3 * 36 + 33, 6, 6);
    ctxOfMain.fillRect(7 * 36 + 33, 7 * 36 + 33, 6, 6);
    ctxOfMain.fillRect(11 * 36 + 33, 11 * 36 + 33, 6, 6);

    return ctxOfMain;
}

/*点击棋盘开始下棋*/
canvasOfMain.onclick = function click(e) {
    console.log("鼠标点击的坐标(" + e.clientX + "," + e.clientY + ")");

    if (isGameOver === false) {
        //获取棋盘相对外边框左边和顶边的偏移量，即确定棋盘左上角那个点的坐标
        var l = this.offsetLeft + 36;
        var t = this.offsetTop + 36;

        //获取点击的位置相对棋盘左上角那个点的坐标的偏移量
        var x = e.clientX - l;
        var y = e.clientY - t;

        var row = 0, col = 0;

        //确定棋子二维数组坐标，左上角为(0,0)，右下角为(14,14)
        if (x > 0) {
            if (x % 36 < 18) {
                row = parseInt(x / 36);
            } else {
                row = parseInt(x / 36) + 1;
            }
        }

        if (y > 0) {
            if (y % 36 < 18) {
                col = parseInt(y / 36);
            } else {
                col = parseInt(y / 36) + 1;
            }
        }

        console.log("棋盘坐标(" + row + "," + col + ")");
        drop(col, row); //棋盘横竖和二维数组的行列需要反一下
    } else {
        toast("比赛结束，请点击重新开始！");
    }
};

/*落子*/
function drop(row, col) {
    if (curCbArray[row][col] === 0) {
        if (isBlack) {//下黑子

            ctxOfMain.drawImage(black, col * 36 + 18, row * 36 + 18);//理论上是加20，这里加22属于微调，让棋子摆在正中央
            isBlack = false;
            hasPiece++;
            revokeFlag++;
            if( revokeFlag >= 2 ){
                var button_revoke1 = document.getElementById('revoke');
                button_revoke1.disabled = false;
            }
            if( hasPiece >= 2){
                var button_giveup1 = document.getElementById('giveup');
                button_giveup1.disabled = false;
            }
            arrayCopy(llastCbArray, lastCbArray);
            arrayCopy(lastCbArray, curCbArray);
            curCbArray[row][col] = 1; //黑子为1
            check(1, row, col);
        } else {

            ctxOfMain.drawImage(white, col * 36 + 18, row * 36 + 18);
            isBlack = true;
            hasPiece++;
            revokeFlag++;
            if( revokeFlag >= 2 ){
                var button_revoke2 = document.getElementById('revoke');
                button_revoke2.disabled = false;
            }
            if( hasPiece >= 2){
                var button_giveup2 = document.getElementById('giveup');
                button_giveup2.disabled = false;
            }
            arrayCopy(llastCbArray, lastCbArray);
            arrayCopy(lastCbArray, curCbArray);
            curCbArray[row][col] = 2; //白子为2
            check(2, row, col);
        }
    }
}

/*判断棋局是否结束*/
function check(color, row, col) {
    var rowBak = row, colBak = col, total = 1;

    //判断东西方向（←→）是否有五个
    while (col > 0 && curCbArray[row][col - 1] === color) {  //当前棋子左边还有几个同色的棋子
        total++;
        col--;
    }
    row = rowBak;
    col = colBak;
    while (col + 1 < 15 && curCbArray[row][col + 1] === color) {  //当前棋子右边还有几个同色的棋子
        col++;
        total++;
    }
    isWin();

    //判断南北方向（↑↓）是否有五个
    reset();
    while (row > 0 && curCbArray[row - 1][col] === color) {   //当前棋子上边还有几个同色的棋子
        total++;
        row--;
    }
    row = rowBak;
    col = colBak;
    while (row + 1 < 15 && curCbArray[row + 1][col] === color) {  //当前棋子下边还有几个同色的棋子
        total++;
        row++;
    }
    isWin();

    //判断东南和西北方向（↖↘）是否有五个
    reset();
    while (row > 0 && col > 0 && curCbArray[row - 1][col - 1] === color) { //当前棋子左上边还有几个同色的棋子
        row--;
        col--;
        total++;
    }
    row = rowBak;
    col = colBak;
    while (row + 1 < 15 && col + 1 < 15 && curCbArray[row + 1][col + 1] === color) {  //当前棋子右下边还有几个同色的棋子
        row++;
        col++;
        total++;
    }
    isWin();

    //判断东北和西南方向（↙↗）有五个
    reset();
    while (row > 0 && col + 1 < 15 && curCbArray[row - 1][col + 1] === color) {  //当前棋子右上边还有几个同色的棋子
        row--;
        col++;
        total++;
    }
    row = rowBak;
    col = colBak;
    while (row + 1 < 15 && col > 0 && curCbArray[row + 1][col - 1] === color) {   //当前棋子左下边还有几个同色的棋子
        row++;
        col--;
        total++;
    }
    isWin();

    //判断是否结束
    function isWin() {
        if (total >= 5) {
            if (color === 1) {
                // alert("黑子赢");
                // canvasOfStatusBar[0].innerHTML="黑子赢";
                ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
                ctxOfStatusBar.fillText("黑子赢", 0, 100);
                isGameOver = true;
                // toast("比赛结束，黑子获胜！");
            } else {
                // alert("白子赢");
                // canvasOfStatusBar[0].innerHTML="白子赢";
                ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
                ctxOfStatusBar.fillText("白子赢", 0, 100);
                isGameOver = true;
                // toast("比赛结束，白子获胜！");
            }
        }
    }

    //重置当前棋子的坐标和连子数
    function reset() {
        rowBak = row;
        colBak = col;
        total = 1;
    }
}

/*重新开始*/
function restart() {
    for (var i = 0; i < 15; i++) {
        for (var j = 0; j < 15; j++) {
            if (curCbArray[i][j] !== 0) {
                curCbArray[i][j] = 0;
            }
            if (lastCbArray[i][j] !== 0) {
                lastCbArray[i][j] = 0;
            }
            if (llastCbArray[i][j] !== 0) {
                llastCbArray[i][j] = 0;
            }
        }
    }

    //重新画棋盘
    ctxOfMain.clearRect(0, 0, 640, 640);
    drawChessboard(ctxOfMain);

    ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
    isGameOver = false;
    isBlack = true;
    hasPiece = 0;
}

/*悔棋*/
function revoke() {
    //重新画棋盘
    ctxOfMain.clearRect(0, 0, 640, 640);
    drawChessboard(ctxOfMain);

    //重新摆子
    for (var i = 0; i < 15; i++) {
       for (var j = 0; j < 15; j++) {
           if( llastCbArray[i][j] === 1 ){
               ctxOfMain.drawImage(black, j * 36 + 18, i * 36 + 18);
           }else if( llastCbArray[i][j] === 2 ) {
               ctxOfMain.drawImage(white, j * 36 + 18, i * 36 + 18);
           }
       }
   }

    /*回退当前棋盘状态，清空前两次棋盘状态*/
    arrayCopy(curCbArray, llastCbArray);
    arrayReset(llastCbArray);
    arrayReset(lastCbArray);

    /*减两颗棋子*/
    if( hasPiece <= 1 ){
        isBlack = true;
        hasPiece = 0;
    }else if( hasPiece >= 2 ){
        hasPiece -= 2;
    }

    revokeFlag = 0;

    /*悔棋按钮禁止按，因为只能悔一步*/
    var button = document.getElementById('revoke');
    button.disabled = true;
}

function giveup() {
    if( isBlack ){
        ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
        ctxOfStatusBar.fillText("白子赢", 0, 100);
        if( confirm("黑子认输，白子获胜!要重新开始一局吗？") ){
            restart();
        }else{
            isGameOver = true;
        }
    }else {
        ctxOfStatusBar.clearRect(0, 0, canvasOfStatusBar[0].width, canvasOfStatusBar[0].height);
        ctxOfStatusBar.fillText("黑子赢", 0, 100);
        if( confirm("白子认输，黑子获胜!要重新开始一局吗？") ){
            restart();
        }else{
            isGameOver = true;
        }
    }
}

/*toast提示*/
function toast(msg) {
    setTimeout(function () {
        document.getElementsByClassName('toast-wrap')[0].getElementsByClassName('toast-msg')[0].innerHTML = msg;
        var toastTag = document.getElementsByClassName('toast-wrap')[0];
        toastTag.className = toastTag.className.replace('toastAnimate', '');
        setTimeout(function () {
            toastTag.className = toastTag.className + ' toastAnimate';
        }, 100);
    }, 500);
}

/*人机模式下判断用户选择的是黑子还是白子*/
function selectColor() {
    var radio = document.getElementsByName("radio-color");
    for (var i = 0; i < radio.length; i++) {
        if (radio[i].checked) {
            //TODO
        }
    }
}