class_name Unit
extends CharacterBody2D

@export var Speed: int = 50
@export var StartMovingTime: float = 1.0
@export var StopMovingTime: float = 1.0

enum State {Idle, Move, Attack}
enum MoveState {Acceleration, FullSpeed, Deceleration}

var state : State = State.Idle
var move_state: MoveState = MoveState.Acceleration
var target: Marker2D
var start_moving_threshold = 10 

var acceleration_time: float = 0.0

func _init() -> void:
	pass

func _process(delta: float) -> void:
	if state != State.Move:
		return
	if move_state == MoveState.Acceleration:
		acceleration_time += delta
		if acceleration_time >= StartMovingTime:
			move_state = MoveState.FullSpeed
	
	var distance_to_target: float = global_position.distance_to(target.global_position)
	if distance_to_target > start_moving_threshold:
		var direction: Vector2 = global_position.direction_to(target.global_position)
		var norm_coef = smoothstep(0.0, StartMovingTime, acceleration_time) 
		var currentSpeed = ease(norm_coef, 0.3) * Speed
		move_and_collide(direction * currentSpeed * delta)
	return


func start_moving() -> void:
	if state == State.Move:
		return

	state = State.Move
	move_state = MoveState.Acceleration
	return

func setTarget(marker: Marker2D) -> void:
	target = marker
