$(document).ready(function(){
	//inițializăm elementul
	$('.slider-maner').draggable({
		//fortam manerul să rămână în interiorul tijei
		containment:'parent',
		//glisare verticala
		axis:'y',
		stop: function(e,ui){
			$('.outputText').load('send.php?servo=' + this.id + '&pos=' + ui.position.top);
			console.log(ui.position.top + ' id:' +  this.id);
		}
	});
});
