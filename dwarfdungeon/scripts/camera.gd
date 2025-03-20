extends Camera2D

var GroupToFollow: Group

func _process(_delta: float) -> void:
	if not GroupToFollow:
		var group = get_parent().get_node_or_null("EntityManager/Group");
		if group == null:
			return
		else:
			GroupToFollow = group
	position = lerp(position, GroupToFollow.position, 0.08)
	return
