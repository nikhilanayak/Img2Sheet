document.getElementById("fake-file-input").addEventListener("click", (e) => {
    document.getElementById("file-input").click();
});


const downloadURL = (data, fileName) => {
    const a = document.createElement('a')
    a.href = data
    a.download = fileName
    document.body.appendChild(a)
    a.style.display = 'none'
    a.click()
    a.remove()
}

const downloadBlob = (data, fileName, mimeType) => {

    const blob = new Blob([data], {
        type: mimeType
    })

    const url = window.URL.createObjectURL(blob)

    downloadURL(url, fileName)

    setTimeout(() => window.URL.revokeObjectURL(url), 1000)
}

var Module = {
    preRun: [],
    postRun: [],
    print: (function () {
        return function (text) {
            console.log(text);
        };
    })(),
    printErr: function (text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        console.error(text);
    },
    setStatus: function (text) {

    },
    totalDependencies: 0,
    monitorRunDependencies: function (left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies - left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    }
};
Module.setStatus('Downloading...');
window.onerror = function (event) {
    Module.setStatus('Exception thrown, see JavaScript console');
    Module.setStatus = function (text) {
        if (text) Module.printErr('[post-exception status] ' + text);
    };
};


document.getElementById("file-input").addEventListener("change", (e) => {
    const file = document.getElementById("file-input").files[0];
    if (!file) {
        return;
    }
    window.file = file;

    const reader = new FileReader();
    reader.onload = (e) => {
        console.log("uploaded");
        const data = new Uint8Array(e.target.result);
        window.data = data;
        let writestream = FS.open(file.name, "w");
        FS.write(writestream, data, 0, data.length, 0);
        FS.close(writestream);

        let scale = document.getElementById("scale").value;
        if (scale == "") {
            scale = 1.0;
        }

        const width = Module.ccall("load_img", "int", ["string", "float"], [file.name, scale]);
        if (width == -1) {
            M.toast({ "html": `${file.name} could not be loaded as an image. Make sure it is a PNG or JPEG image`, "displayLength": 8000 });
            return;
        }
        else if (width == -2) {
            M.toast({
                "html": "Scale must be greater than 0", "displayLength": 8000
            });
            return;
        }
        console.log("width: ", width);

        let curr = 0;
        const handle = setInterval(() => {
            curr += 1;

            if (!Module.ccall("next", "boolean", ["void"])) {
                progress(100);
                const infoElement = document.createElement("p");
                document.body.appendChild(infoElement);
                Module.ccall("save", "void", ["void"]);

                const thread_handle = setInterval(() => {
                    if (Module.ccall("is_done", "boolean", ["void"], null)) {
                        console.log("done!");
                        clearInterval(thread_handle);

                        let file_size = FS.stat("excel.xlsx").size;


                        const readstream = FS.open("excel.xlsx", "r");
                        const outdata = new Uint8Array(file_size);
                        FS.read(readstream, outdata, 0, file_size, 0);
                        FS.close(readstream);
                        downloadBlob(outdata, "file.xlsx", "application/vnc.ms-excel");
                        progress(0);
                        infoElement.remove();

                    }
                }, 10);

                clearInterval(handle);
                return;
            }
            progress(curr / width * 100);
        }, 0);



    }
    reader.readAsArrayBuffer(file);
    document.getElementById("file-input").value = "";
}, false);

const progress = (percent) => {
    document.getElementById("progress").style.width = percent + "%";
}