extends RichTextLabel

var time_start = 0
var time_now = 0

func _ready():
	time_start = OS.get_unix_time()
	set_process(true)

func _process(delta):
	time_now = OS.get_unix_time()
	var elapsed = time_now - time_start
	var seconds = elapsed % 60
	var minutes = elapsed / 60
	var hours = minutes / 24;
	var str_elapsed = "%02d hours : %02d min : %02d sec" % [hours, minutes, seconds]
	var text = "Running Time : " + str_elapsed
	set_text("Running Time: " + str_elapsed);
