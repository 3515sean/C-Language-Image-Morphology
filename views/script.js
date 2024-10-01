const form = document.getElementById('uploadForm');
const uploadBtn = document.getElementById('uploadBtn');
const progressBar = document.getElementById('progressBar');
const resultDiv = document.getElementById('result');
var image_path;
form.addEventListener('submit', e => {
  e.preventDefault();
  uploadBtn.disabled = true;
  progressBar.style.display = 'block';
  const formData = new FormData(form);
  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/upload', true);
  xhr.upload.onprogress = e => {
    const progress = (e.loaded / e.total) * 80;
    progressBar.style.width = progress + '%';
  };
  xhr.onreadystatechange = () => {
    if (xhr.readyState === XMLHttpRequest.DONE) {
      progressBar.style.display = 'none';
      uploadBtn.disabled = false;
      if (xhr.status === 200) {
        resultDiv.style.display = 'block';
        resultDiv.src = xhr.responseText;
		image_path = xhr.responseText;
      } else {
        alert('An error occurred while uploading the image.');
      }
    }
  };
  xhr.send(formData);
});
document.getElementById("BinaryBtn").addEventListener("click", function() {
	LoadImage(image_path + "?process=binary",resultDiv);
});
document.getElementById("DilationBtn").addEventListener("click", function() {
	LoadImage(image_path + "?process=dilation",resultDiv);
});
document.getElementById("ErosionBtn").addEventListener("click", function() {
	LoadImage(image_path + "?process=erosion",resultDiv);
});
document.getElementById("OpeningBtn").addEventListener("click", function() {
	LoadImage(image_path + "?process=opening",resultDiv);
});
document.getElementById("ClosingBtn").addEventListener("click", function() {
	LoadImage(image_path + "?process=closing",resultDiv);
});

function LoadImage(url,element) {
    var img = new Image();

    img.onload = function() {
        element.src = img.src;
    };

    img.src = url + "&date=" + new Date().getTime();
}