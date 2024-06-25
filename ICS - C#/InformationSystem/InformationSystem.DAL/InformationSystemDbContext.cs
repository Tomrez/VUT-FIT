using InformationSystem.DAL.Entities;
using InformationSystem.DAL.Seeds;
using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InformationSystem.DAL
{
    public class InformationSystemDbContext(DbContextOptions contextOptions, bool seedDemoData = false) : DbContext(contextOptions)
    {
        public DbSet<SubjectEntity> Subjects => Set<SubjectEntity>();
        public DbSet<StudentEntity> Students => Set<StudentEntity>();
        public DbSet<ActivityEntity> Activities => Set<ActivityEntity>();
        public DbSet<StudentEvaluationEntity> StudentEvaluation => Set<StudentEvaluationEntity>();
        public DbSet<StudentsInSubjectEntity> StudentsInSubject => Set<StudentsInSubjectEntity>();

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.Entity<StudentEntity>()
                .HasMany(act => act.ActivityEvaluation)
                .WithOne(act => act.Student)
                .OnDelete(DeleteBehavior.Restrict);

            modelBuilder.Entity<ActivityEntity>()
                .HasMany<StudentEvaluationEntity>()
                .WithOne(act => act.Activity)
                .OnDelete(DeleteBehavior.Restrict);

            modelBuilder.Entity<SubjectEntity>()
                .HasMany<StudentsInSubjectEntity>()
                .WithOne(subj => subj.Subject)
                .OnDelete(DeleteBehavior.Restrict);

            modelBuilder.Entity<StudentEntity>()
                .HasMany(stud => stud.Subjects)
                .WithOne(stud => stud.Student)
                .OnDelete(DeleteBehavior.Cascade);

            modelBuilder.Entity<SubjectEntity>()
                .HasMany(act => act.Activities)
                .WithOne(act => act.Subject)
                .OnDelete(DeleteBehavior.Cascade);

            if (seedDemoData)
            {
                StudentSeeds.Seed(modelBuilder);
                SubjectSeeds.Seed(modelBuilder);
                ActivitySeeds.Seed(modelBuilder);
                StudentsInSubjectSeeds.Seed(modelBuilder);
                //StudentEvaluationSeeds.Seed(modelBuilder);
            }
        }
    }
}
