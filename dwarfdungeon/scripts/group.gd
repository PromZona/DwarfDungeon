class_name Group
extends Node2D

@export var Speed: float = 50.0
@export var Units: Array[Unit] = []

@onready var position1: Marker2D = $Position1
@onready var position2: Marker2D = $Position2
@onready var position3: Marker2D = $Position3
@onready var position4: Marker2D = $Position4
var positions: Array[Marker2D] = []

func _ready() -> void:
	positions.append($Position1)
	positions.append($Position2)
	positions.append($Position3)
	positions.append($Position4)
	
	var i: int = 0
	for unit: Unit in Units:
		unit.setTarget(positions[i])
		i += 1
	return

func _process(delta: float) -> void:
	var direction: Vector2 = Vector2.ZERO
	if Input.is_action_pressed("move_left"):
		direction.x -= 1.0
	if Input.is_action_pressed("move_right"):
		direction.x += 1.0
	if Input.is_action_pressed("move_up"):
		direction.y -= 1.0
	if Input.is_action_pressed("move_down"):
		direction.y += 1.0
	
	position += direction.normalized() * Speed * delta
	return


func _input(event: InputEvent) -> void:
	return
