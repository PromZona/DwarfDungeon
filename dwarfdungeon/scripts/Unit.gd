class_name Unit
extends CharacterBody2D

@export var Speed: int = 50
@export var StartMovingTime: float = 1.0
@export var StopMovingTime: float = 1.0
@export var DecelerationDistance: float = 50.0

@onready var default_font = ThemeDB.fallback_font

enum State {Idle, Move, Attack}
enum MoveState {Acceleration, FullSpeed, Deceleration}

var state : State = State.Idle
var move_state: MoveState = MoveState.Acceleration
var target: Marker2D
var distance_to_target: float 

var acceleration_time: float = 0.0
var current_deceleration_distance = 0.0

func get_speed() -> float:
	if state != State.Move:
		return 0.0
	
	var coef: float = 1.0
	if move_state == MoveState.Acceleration:
		coef *= ease(smoothstep(0.0, StartMovingTime, acceleration_time), 0.3)
	elif move_state == MoveState.Deceleration:
		coef *= ease(smoothstep(0.0, StopMovingTime, current_deceleration_distance), 0.3)
	
	return Speed * coef

func update_move_state(delta: float) -> void:
	if move_state == MoveState.Acceleration:
		acceleration_time += delta
		if acceleration_time >= StartMovingTime:
			move_state = MoveState.FullSpeed
		return
	if move_state == MoveState.FullSpeed:
		if distance_to_target < DecelerationDistance:
			move_state = MoveState.Deceleration
			acceleration_time = StopMovingTime
		return
	if move_state == MoveState.Deceleration:
		if distance_to_target > DecelerationDistance:
			move_state = MoveState.FullSpeed
			return
		current_deceleration_distance = DecelerationDistance - distance_to_target
		if distance_to_target <= 5.0:
			state = State.Idle
		return


func get_final_velocity() -> Vector2:
	var result: Vector2 = Vector2.ZERO
	# group force
	# formation force
	# unit force
	return result

func _init() -> void:
	pass

func _process(delta: float) -> void:
	distance_to_target = global_position.distance_to(target.global_position)
	queue_redraw()
	if state != State.Move:
		return
	update_move_state(delta)
	var direction: Vector2 = global_position.direction_to(target.global_position)
	var currentSpeed = get_speed()
	move_and_collide(direction * currentSpeed * delta)
	
	return

func _draw() -> void:
	var color: Color = Color.WHITE
	if state == State.Idle:
		color = Color.YELLOW
	if state == State.Move:
		color = Color.BLUE
	draw_string(default_font, Vector2(-15, 25), State.keys()[state], HORIZONTAL_ALIGNMENT_LEFT, -1, 10, color)

	if move_state == MoveState.Acceleration:
		color = Color.GREEN
	if move_state == MoveState.FullSpeed:
		color = Color.WHITE
	if move_state == MoveState.Deceleration:
		color = Color.RED
	draw_string(default_font, Vector2(-15, 40), MoveState.keys()[move_state], HORIZONTAL_ALIGNMENT_LEFT, -1, 10, color)
	draw_line(Vector2(0, 0), Vector2(100,100),Color.GREEN) 
	return

func start_moving() -> void:
	if state == State.Move:
		return
	state = State.Move
	move_state = MoveState.Acceleration
	return

func setTarget(marker: Marker2D) -> void:
	target = marker
