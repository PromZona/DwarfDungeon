extends Camera2D

@export var GroupToFollow: Group

func _process(_delta: float) -> void:
	position = lerp(position, GroupToFollow.position, 0.08)
	return
