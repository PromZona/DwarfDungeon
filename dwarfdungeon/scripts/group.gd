class_name Group
extends Node2D

@export var Speed: float = 50.0
@export var Units: Array[Unit] = []

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
	if CurrentState != GroupState.GroupMoving:
		var start_moving: bool = false
		start_moving = start_moving || event.is_action_pressed("move_left")
		start_moving = start_moving || event.is_action_pressed("move_right")
		start_moving = start_moving || event.is_action_pressed("move_up")
		start_moving = start_moving || event.is_action_pressed("move_down")
		if start_moving:
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
	
	# if direction.is_zero_approx():

	position += direction.normalized() * Speed * delta
	return

func attack():
	pass


func _process(delta: float) -> void:
	if CurrentState == GroupState.GroupMoving:
		move(delta)
	return

