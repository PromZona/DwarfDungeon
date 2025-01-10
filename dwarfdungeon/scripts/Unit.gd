class_name Unit
extends CharacterBody2D

@export var Speed: int = 50

var target: Marker2D

func _init() -> void:
	pass

func _process(delta: float) -> void:
	var direction: Vector2 = global_position.direction_to(target.global_position)
	move_and_collide(direction * Speed * delta)
	pass

func setTarget(marker: Marker2D) -> void:
	target = marker
