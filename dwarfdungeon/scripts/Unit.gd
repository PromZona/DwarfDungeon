class_name Unit
extends CharacterBody2D

@export_group("Movement")
@export var Speed: int = 50
@export_subgroup("Fomation", "Formation")
@export var FormationCloseEnoughDistance: float = 20.0
@export var FormationMaxSpeedDistance: float = 500.0
@export_range(0, 1, 0.05) var FormationMinSpeedCoefficient: float = 0.1
@export var FormationMaxSpeedCoefficient: float = 2.0

@onready var DefaultFont = ThemeDB.fallback_font

enum State {Idle, Move, Attack}
var CurrentState : State = State.Idle
var Target: Marker2D
var GroupForceDirection: Vector2 = Vector2.ZERO 

func _process(_delta: float) -> void:
	queue_redraw()
	return

func _physics_process(delta: float) -> void:
	move_and_collide(get_final_velocity() * delta)
	return

func _draw() -> void:
	# draw_string(DefaultFont, Vector2(-20, 35), "g: %.1v" % group_velocity(), HORIZONTAL_ALIGNMENT_FILL, -1, 9, Color.WHITE)
	# draw_string(DefaultFont, Vector2(-20, 48), "f: %.1v" % formation_velocity(), HORIZONTAL_ALIGNMENT_FILL, -1, 9, Color.WHITE)
	return

func formation_velocity() -> Vector2:
	var direction: Vector2 = global_position.direction_to(Target.global_position)
	var distance: float = global_position.distance_to(Target.global_position)
	
	if distance < FormationCloseEnoughDistance: # If close "enough", remove jittering around
		return Vector2.ZERO

	var distance_normilized: float = smoothstep(0, FormationMaxSpeedDistance, distance)	# linear [0; FormationMaxSpeedDistance] -> [0; 1]
	distance_normilized = clampf(distance_normilized, FormationMinSpeedCoefficient, 1.0)	# clamp  [0; 1] -> [FormationMinSpeedCoefficient; 1] 
	var distance_velocity: float = lerpf(0.0, Speed * FormationMaxSpeedCoefficient, distance_normilized) # [0.1; 1] -> [0.0; Speed] 
	return direction * distance_velocity

func group_velocity() -> Vector2:
	return GroupForceDirection * Speed

func get_final_velocity() -> Vector2:
	# group force
	var result: Vector2 = group_velocity() # TODO: Make group coef common
	# formation force
	result += formation_velocity()
	# unit force TODO:
	
	return result

func SetTarget(marker: Marker2D) -> void:
	Target = marker

func SetGroupForceDirection(direction: Vector2) -> void:
	GroupForceDirection = direction
