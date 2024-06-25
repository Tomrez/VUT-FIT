using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InformationSystem.DAL.Entities
{
    public record StudentEntity : EntityBase
    {
        public required string Login { get; set; }
        public required string Name { get; set; }
        public required string Surname { get; set; }
        public Uri? PhotoUrl { get; set; }

        public ICollection<StudentsInSubjectEntity> Subjects { get; set; } = new List<StudentsInSubjectEntity>();
        public ICollection<StudentEvaluationEntity> ActivityEvaluation { get; set; } = new List<StudentEvaluationEntity>();
    }
}