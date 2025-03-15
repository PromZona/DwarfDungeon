class_name Unit
extends CharacterBody2D

@export var Damage: float = 2.0
@export_group("Movement")
@export var Speed: int = 50
@export_subgroup("Fomation", "Formation")
@export var FormationCloseEnoughDistance: float = 20.0
@export var FormationMaxSpeedDistance: float = 500.0
@export_range(0, 1, 0.05) var FormationMinSpeedCoefficient: float = 0.1
@export var FormationMaxSpeedCoefficient: float = 2.0

@onready var DefaultFont = ThemeDB.fallback_font
@onready var PersonalSpaceArea: Area2D = $GroupMemberCollision
@onready var AttackArea: ShapeCast2D = $AttackAreaHandler/AttackArea
@onready var AttackAreaHandler: Node2D = $AttackAreaHandler
@onready var AnimPlayer: AnimationPlayer = $AnimationPlayer

enum State {Idle, Move, Attack}
var CurrentState : State = State.Idle
var Target: Marker2D
var GroupForceDirection: Vector2 = Vector2.ZERO 
var GroupUnitsTooClose: Array[Area2D] = []
var PersonalSpaceRadius: float
var ClosestEnemy: CharacterBody2D

func _ready() -> void:
	var colshape2d: CollisionShape2D = PersonalSpaceArea.get_child(0)
	PersonalSpaceRadius = (colshape2d.shape as CircleShape2D).radius
	return

func _process(_delta: float) -> void:
	queue_redraw()
	
	return

func _physics_process(delta: float) -> void:
	move_and_collide(get_final_velocity() * delta)
	return

func _draw() -> void:
	# draw_line(Vector2.ZERO, formation_velocity(), Color.RED)
	# draw_line(Vector2.ZERO, group_units_velocity(), Color.GREEN)
	# draw_line(Vector2.ZERO, group_velocity(), Color.MAGENTA)
	# draw_string(DefaultFont, Vector2(-20, 35), "dist: %.1f" % global_position.distance_to(Target.global_position), HORIZONTAL_ALIGNMENT_FILL, -1, 9, Color.WHITE)
	# draw_string(DefaultFont, Vector2(-20, 48), "f: %.1v" % formation_velocity(), HORIZONTAL_ALIGNMENT_FILL, -1, 9, Color.WHITE)
	return

func formation_velocity() -> Vector2:
	var direction: Vector2 = global_position.direction_to(Target.global_position)
	var distance: float = global_position.distance_to(Target.global_position)
	
	if distance < FormationCloseEnoughDistance: # If close "enough", remove jittering around
		return Vector2.ZERO

	var distance_normilized: float = smoothstep(0, FormationMaxSpeedDistance, distance)	# linear [0; FormationMaxSpeedDistance] -> [0; 1]
	distance_normilized = clampf(distance_normilized, FormationMinSpeedCoefficient, 1.0)	# clamp  [0; 1] -> [FormationMinSpeedCoefficient; 1] 
	var magnitude: float = lerpf(0.0, Speed * FormationMaxSpeedCoefficient, distance_normilized) # [0.1; 1] -> [0.0; Speed] 
	return direction * magnitude

func group_velocity() -> Vector2:
	return GroupForceDirection * Speed

func group_units_velocity() -> Vector2:
	var result: Vector2 = Vector2.ZERO
	for area: Area2D in GroupUnitsTooClose:
		var opposite_dir: Vector2 = -1 * PersonalSpaceArea.global_position.direction_to(area.global_position)
		var distance_to_area: float = PersonalSpaceArea.global_position.distance_to(area.global_position)
		var distance_normilized: float = 1.0 - smoothstep(0, 2 * PersonalSpaceRadius, distance_to_area) # [0; 2*Radius] -> [1; 0]
		var magnitude: float =  lerpf(0.0, Speed * 0.2, distance_normilized)
		result += opposite_dir * magnitude
	return result

func get_final_velocity() -> Vector2:
	# group force
	var result: Vector2 = group_velocity()

	# formation force
	result += formation_velocity()
	
	# unit force
	result += group_units_velocity()
	return result

func SetTarget(marker: Marker2D) -> void:
	Target = marker
	return

func SetGroupForceDirection(direction: Vector2) -> void:
	GroupForceDirection = direction
	return

func apply_damage() -> void:
	AttackArea.force_shapecast_update()
	var collisions: Array = AttackArea.collision_result # Thank you godot for static typing
	for col in collisions:
		var enemy: EnemyView = col.collider as EnemyView
		if enemy:
			enemy.ReceiveDamage(Damage)
	return

func Attack(direction: Vector2) -> void:
	AttackAreaHandler.look_at(direction)
	AnimPlayer.play("Attack")
	return

func _on_group_member_collision_area_entered(area: Area2D) -> void:
	GroupUnitsTooClose.append(area)
	return

func _on_group_member_collision_area_exited(area: Area2D) -> void:
	GroupUnitsTooClose.erase(area)
	return
