<?php

namespace IPP\Student;

/**
 * Trida Variable
 * Trida slouzici k uchovavani informaci o promenne
 * @param $name slouzi k ulozeni nazvu promenne
 * @param $value slouzi k ulozeni hodnoty promenne, pokud byla zadana
 * @param $type slouzi k uchovani informace o datovem typu promenne, pokud byl nekde explicitne dan
 * @param $hasValue je kinda hotfix promenna, ktera slouzi ke kontrole, zda se v promenne neco nachazi pri volani instrukce WRITE. Rozlisuje rozdil mezi prazdnym stringem a nedefinovanou hodnotou
 */
class Variable 
{
    private string $name;
    private ?string $value;
    private ?string $type;
    private bool $hasValue;
    public function __construct(string $name, ?string $value, ?string $type)
    {
        $this->name = $name;
        $this->value = $value;
        $this->type = $type;
        if ($value != null)
        {
            $this->hasValue = true;
        }
        else
        {
            $this->hasValue = false;
        }
    }
    /** Funkce getName vraci nazev promenne, nad kterou byla zavolana */
    public function getName(): string
    {
        return $this->name;
    }
    /** Funkce getValue vraci hodnotu promenne, nad kterou byla zavolana */
    public function getValue(): ?string
    {
        return $this->value;
    }
    /** Funkce getName vraci typ promenne, nad kterou byla zavolana */
    public function getType(): ?string
    {
        return $this->type;
    }

    public function isVariable(Argument $arg): bool
    {
        if($arg->getType() == "var")
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public function updateValue(mixed $value): void
    {
        $this->value = $value;
        $this->hasValue = true;
    }

    public function updateType(string $type): void
    {
        $this->type = $type;
    }

    public function hasValue(): bool
    {
         return $this->hasValue;
    }
}