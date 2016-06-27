<script type="text/javascript">

/****************************************************
*	        DOM Image rollover:
*		by Chris Poole
*		http://chrispoole.com
*               Script featured on http://www.dynamicdrive.com
*		Keep this notice intact to use it :-)
****************************************************/

function init() {
  if (!document.getElementById) return
  var imgOriginSrc;
  var imgTemp = new Array();
  var imgarr = document.getElementsByTagName('img');
  for (var i = 0; i < imgarr.length; i++) {
    if (imgarr[i].getAttribute('hsrc')) {
        imgTemp[i] = new Image();
        imgTemp[i].src = imgarr[i].getAttribute('hsrc');
        imgarr[i].onmouseover = function() {
            imgOriginSrc = this.getAttribute('src');
            this.setAttribute('src',this.getAttribute('hsrc'))
        }
        imgarr[i].onmouseout = function() {
            this.setAttribute('src',imgOriginSrc)
        }
    }
  }
}
onload=init;

</script>
