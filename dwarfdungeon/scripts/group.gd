class_name Group
extends Node2D

@export var Speed: float = 50.0
@export var Units: Array[Unit] = []

@onready var default_font = ThemeDB.fallback_font
@onready var default_font_size = ThemeDB.fallback_font_size
	
enum GroupState {GroupIdle, GroupMoving, GroupAttack}

var Positions: Array[Marker2D] = []
var CurrentState: GroupState = GroupState.GroupIdle

func _ready() -> void:
	Positions.append($Position1)
	Positions.append($Position2)
	Positions.append($Position3)
	Positions.append($Position4)
	
	var i: int = 0
	for unit: Unit in Units:
		unit.setTarget(Positions[i])
		i += 1
	return

func notify_unit_moving():
	for unit: Unit in Units:
		unit.start_moving()
	return

func _input(event: InputEvent) -> void:
	var is_moving: bool = false
	is_moving = is_moving || event.is_action_pressed("move_left")
	is_moving = is_moving || event.is_action_pressed("move_right")
	is_moving = is_moving || event.is_action_pressed("move_up")
	is_moving = is_moving || event.is_action_pressed("move_down")

	if CurrentState == GroupState.GroupIdle && is_moving:
		CurrentState = GroupState.GroupMoving
		notify_unit_moving()
		return
	return

func move(delta: float) -> void:
	var direction: Vector2 = Vector2.ZERO
	if Input.is_action_pressed("move_left"):
		direction.x -= 1.0
	if Input.is_action_pressed("move_right"):
		direction.x += 1.0
	if Input.is_action_pressed("move_up"):
		direction.y -= 1.0
	if Input.is_action_pressed("move_down"):
		direction.y += 1.0
	
	if direction.is_zero_approx():
		CurrentState = GroupState.GroupIdle

	position += direction.normalized() * Speed * delta
	return

func attack():
	pass


func _process(delta: float) -> void:
	if CurrentState == GroupState.GroupMoving:
		move(delta)
	queue_redraw()
	return

func _draw() -> void:
	# var str_pos: Vector2  = Vector2(-80, -120)
	# var str_format_state: String = "State: %s"
	# var str_format_move_state: String = "MoveState: %s"
	# for unit: Unit in Units:
	#	var text: String = str_format_state % unit.State.keys()[unit.state]
	#	draw_string(default_font, str_pos, text, HORIZONTAL_ALIGNMENT_LEFT, -1, default_font_size)
	#	str_pos.y += 20
	#	text = str_format_move_state % unit.MoveState.keys()[unit.move_state]
	#	draw_string(default_font, str_pos, text, HORIZONTAL_ALIGNMENT_LEFT, -1, default_font_size)
	#	str_pos.y += 50
	return
