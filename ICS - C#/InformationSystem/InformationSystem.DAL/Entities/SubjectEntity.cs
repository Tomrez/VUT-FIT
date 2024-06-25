namespace InformationSystem.DAL.Entities
{
    public record SubjectEntity : EntityBase
    {
        public required string Name { get; set; }
        public required string Abbreviation { get; set; }

        public ICollection<ActivityEntity> Activities { get; set; } = new List<ActivityEntity>();
    }
}