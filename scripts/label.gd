extends RichTextLabel

onready var net = get_parent().get_parent().get_node("network");

func _process(_delta):
	text = "Rounds: " + str(net.rounds);
