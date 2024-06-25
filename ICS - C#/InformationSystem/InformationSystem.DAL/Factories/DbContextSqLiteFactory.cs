using Microsoft.EntityFrameworkCore;

namespace InformationSystem.DAL.Factories;

public class DbContextSqLiteFactory : IDbContextFactory<InformationSystemDbContext>
{
    private readonly bool _seedTestingData;
    private readonly DbContextOptionsBuilder<InformationSystemDbContext> _contextOptionsBuilder = new();

    public DbContextSqLiteFactory(string databaseName, bool seedTestingData = false)
    {
        _seedTestingData = seedTestingData;

        _contextOptionsBuilder.UseSqlite($"Data Source={databaseName};Cache=Shared");
    }

    public InformationSystemDbContext CreateDbContext() => new(_contextOptionsBuilder.Options, _seedTestingData);
}
