﻿namespace InformationSystem.DAL.Entities
{
    public abstract record EntityBase : IEntity
    {
        public Guid Id { get; set; } = Guid.NewGuid();
    }
}