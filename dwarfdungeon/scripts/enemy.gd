class_name Enemy
extends CharacterBody2D

@export var MaxHealth: float = 100.0

@onready var HealthBar: TextureProgressBar = $HealthBar
@onready var AnimManager: AnimationPlayer = $AnimationPlayer

var CurrentHealth: float

func _ready() -> void:
	CurrentHealth = MaxHealth
	HealthBar.max_value = MaxHealth
	HealthBar.value = CurrentHealth
	return

func DealDamage(damage: float) -> void:
	CurrentHealth -= damage
	HealthBar.value = CurrentHealth
	AnimManager.play("receive_damage")
	return
