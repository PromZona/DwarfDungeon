class_name Group
extends Node2D

@export var Speed: float = 50.0
@export var Units: Array[CharacterBody2D]= []

@onready var Camera: Camera2D = get_viewport().get_camera_2d()

var Positions: Array[Marker2D] = []
var GroupCenter: Vector2 = Vector2.ZERO

func _ready() -> void:
	Positions.append($Position1)
	Positions.append($Position2)
	Positions.append($Position3)
	Positions.append($Position4)
	
	var i: int = 0
	for unit: PlayerUnitView in Units:
		unit.set_target(Positions[i])
		i += 1
	return

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("attack"):
		return
		# GDExtensionManager.reload_extension(GDExtensionManager.get_loaded_extensions()[0])
	return

func _process(delta: float) -> void:
	move(delta)
	rotate_with_mouse()
	return

func _draw() -> void:
	return

func group_center(unit_positions: Array[Vector2]) -> Vector2:
	assert(unit_positions.size() == 4)
	var pair1: Vector2 = (unit_positions[0] + unit_positions[1]) / 2
	var pair2: Vector2 = (unit_positions[2] + unit_positions[3]) / 2
	var result: Vector2 = (pair1 + pair2) / 2
	return result

func move(_delta: float) -> void:
	var direction: Vector2 = Vector2.ZERO
	if Input.is_action_pressed("move_left"):
		direction.x -= 1.0
	if Input.is_action_pressed("move_right"):
		direction.x += 1.0
	if Input.is_action_pressed("move_up"):
		direction.y -= 1.0
	if Input.is_action_pressed("move_down"):
		direction.y += 1.0
	
	direction = direction.normalized()
	var unit_positions: Array[Vector2] = []
	for unit: PlayerUnitView in Units:
		unit.set_group_direction(direction)
		unit_positions.append(unit.global_position)
	var target_position:Vector2 = group_center(unit_positions)
	position = target_position
	return

func rotate_with_mouse() -> void:
	var mouse_pos: Vector2 = Camera.get_global_mouse_position()
	look_at(mouse_pos)
	return
