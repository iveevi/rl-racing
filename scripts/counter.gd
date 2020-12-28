extends RichTextLabel

var nd = null;
var cr = null;

func _ready():
	nd = get_parent().get_parent().get_node("network");
	cr = get_parent().get_parent().get_node("network/car");

func _process(delta):
#	set_text("Round #" + str(nd.rounds)
#	+ "\n\nBrake: " + str(cr.brakes) + "\n\nSteer: " + str(cr.steer)
#	+ "\n\nScore: " + str(cr.score) + "\n\nRt: " + str(cr.Rt)
#	+ "\n\nDist: " + str(cr.dist));
	
	set_text("Round #" + str(nd.rounds)
	+ "\n\nBrake: " + str(cr.brakes) + "\n\nSteer: " + str(cr.steer)
	+ "\n\nDist: " + str(cr.dist));
