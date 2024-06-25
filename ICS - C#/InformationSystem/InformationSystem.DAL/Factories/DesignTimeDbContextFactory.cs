using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;

namespace InformationSystem.DAL.Factories;

public class DesignTimeDbContextFactory : IDesignTimeDbContextFactory<InformationSystemDbContext>
{
    private readonly DbContextSqLiteFactory _dbContextSqLiteFactory = new($"InformationSystem.db");

    public InformationSystemDbContext CreateDbContext(string[] args) => _dbContextSqLiteFactory.CreateDbContext();
}