extends RichTextLabel

func _ready():
	pass;

func _process(delta):
	set_text("Speed: " + String(round(get_parent().get_parent().get_child(1).velocity)));
